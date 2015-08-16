/*
	LuaScriptExports.cpp

	This file contains several internal classes for exporting
	functions from C++ to Lua and for managing events between
	the two language layers. Most functions are wrappers
	for engine functionality and forward requests from Lua to
	the engine.

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "LuaScriptExports.h"

#include <FastDelegate.h>
#include <LuaPlus.h>
#include <memory>
#include <set>
#include <tinyxml.h>

#include "EventManager.h"
#include "Events.h"
#include "GameObject.h"
#include "interfaces.h"
#include "Logger.h"
#include "LuaScriptEvent.h"
#include "LuaStateManager.h"
#include "Matrix.h"
#include "ResourceCache.h"
#include "Vector.h"

/*
	This class is a C++ listener that will listen for events and forward the event fires to a 
	lua callback listener. It pairs an event type with a Lua callback function for that event. 
	The event can be defined in C++ or in Lua, and can be sent from C++ or Lua. 

	The Lua callback function shuld take in a table with the event data.
*/
class LuaScriptEventListener
{
public:
	// create a lua listener for an event type
	explicit LuaScriptEventListener(const EventType& eventType, const LuaPlus::LuaObject& scriptCallbackFunction) :
		m_ScriptCallbackFunction(scriptCallbackFunction), m_EventType(eventType)
	{ }

	// remove the lua listener 
	~LuaScriptEventListener()
	{
		IEventManager* pEventManager = IEventManager::Get();
		if (pEventManager)
			pEventManager->RemoveListener(GetDelegate(), m_EventType);
	}

	// return the delegate listener function for this event
	EventListenerDelegate GetDelegate()
	{
		return fastdelegate::MakeDelegate(this, &LuaScriptEventListener::ScriptEventDelegate);
	}

	// call the lua callback function with the event
	void ScriptEventDelegate(IEventPtr pEventPtr)
	{
		// make sure its actually a lua function
		CB_ASSERT(m_ScriptCallbackFunction.IsFunction()); 
		shared_ptr<LuaScriptEvent> pScriptEvent(std::static_pointer_cast<LuaScriptEvent>(pEventPtr));
		LuaPlus::LuaFunction<void> Callback = m_ScriptCallbackFunction;
		Callback(pScriptEvent->GetEventData());
	}

private:
	EventType m_EventType;
	LuaPlus::LuaObject m_ScriptCallbackFunction;
};


/*
	This class manages C++ LuaScriptEventListener objects.
*/
class LuaScriptEventListenerManager
{
public:
	~LuaScriptEventListenerManager();

	// add a listener to object to the manager
	void AddListener(LuaScriptEventListener* pListener)
	{
		m_Listeners.insert(pListener);
	}

	// destroy a listener
	void DestroyListener(LuaScriptEventListener* pListener)
	{
		// iterate the set looking for the listener to remove
		auto findIt = m_Listeners.find(pListener);
		if (findIt != m_Listeners.end())
		{
			m_Listeners.erase(findIt);
			delete pListener;
		}
		else
		{
			CB_ERROR("Couldn't find the script listener in the set -- Potential Memory Leak");
		}
	}

private:
	std::set<LuaScriptEventListener*> m_Listeners;
};


/*
	This is an internal class for all lua script exported functions. These are 
	wrappers for engine functionality that is exposed to lua.
*/
class LuaInternalScriptExports
{
public:
	// initialization
	static bool Init();
	static void Destroy();

	// script resource loading
	static bool LoadAndExecuteScriptResource(const char* scriptResource);

	// game objects
	static int CreateGameObject(const char* objectArchetype, LuaPlus::LuaObject luaPosition, LuaPlus::LuaObject luaYawPitchRoll);

	// event system
	static unsigned long RegisterEventListener(EventType eventType, LuaPlus::LuaObject callbackFunction);
	static void RemoveEventListener(unsigned long listenerId);
	static bool QueueEvent(EventType eventType, LuaPlus::LuaObject eventData);
	static bool TriggerEvent(EventType eventType, LuaPlus::LuaObject eventData);

	// process system
	static void AttachScriptProcess(LuaPlus::LuaObject scriptProcess);

	// math
	static float GetYRotationFromVector(LuaPlus::LuaObject vec3);
	static float WrapPi(float wrapMe);
	static LuaPlus::LuaObject GetVectorFromRotation(float angleRadians);

	// misc
	static void LuaLog(LuaPlus::LuaObject text);
	static unsigned long GetTickCount();

	// physics
	static void ApplyForce(LuaPlus::LuaObject normalDirectionLua, float force, int gameObjectId);
	static void ApplyTorque(LuaPlus::LuaObject axisLua, float force, int gameObjectId);

private:
	static LuaScriptEventListenerManager* s_pScriptEventListenerMgr;
	static shared_ptr<LuaScriptEvent> BuildEvent(EventType eventType, LuaPlus::LuaObject& eventData);
};

LuaScriptEventListenerManager* LuaInternalScriptExports::s_pScriptEventListenerMgr = nullptr;

// initialize the script export system
bool LuaInternalScriptExports::Init()
{
	CB_ASSERT(s_pScriptEventListenerMgr == nullptr);
	s_pScriptEventListenerMgr = CB_NEW LuaScriptEventListenerManager;
	return true;
}

// destroy the script export system
void LuaInternalScriptExports::Destroy()
{
	CB_ASSERT(s_pScriptEventListenerMgr != nullptr);
	CB_SAFE_DELETE(s_pScriptEventListenerMgr);
}

// load a script resource then execute it. used by the require() function in lua script
bool LuaInternalScriptExports::LoadAndExecuteScriptResource(const char* scriptResource)
{
	if (!g_pApp->m_ResCache->IsUsingDevelopmentDirectories())
	{
		Resource resource(scriptResource);
		shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);
		if (pResourceHandle)
		{
			return true;
		}
		return false;
	}
	else
	{
		// if using development directories, have lua execute the file directly instead of
		// going through the resource cache
		std::string path("..\\Assets\\");
		path += scriptResource;
		LuaStateManager::Get()->ExecuteFile(path.c_str());
		return true;
	}
}

// create a game object from lua
int LuaInternalScriptExports::CreateGameObject(const char* objectArchetype, LuaPlus::LuaObject luaPosition, LuaPlus::LuaObject luaYawPitchRoll)
{
	// lua position must be a table
	if (!luaPosition.IsTable())
	{
		CB_ERROR("Invalid object passed to CreateGameObject(). Type = " + std::string(luaPosition.TypeName()));
		return INVALID_GAMEOBJECT_ID;
	}
	// lua yawPitchRoll must be a table
	if (!luaYawPitchRoll.IsTable())
	{
		CB_ERROR("Invalid object passed to CreateGameObject(). Type = " + std::string(luaYawPitchRoll.TypeName()));
		return INVALID_GAMEOBJECT_ID;
	}

	Vec3 position(luaPosition["x"].GetFloat(), luaPosition["y"].GetFloat(), luaPosition["z"].GetFloat());
	Vec3 yawPitchRoll(luaYawPitchRoll["x"].GetFloat(), luaYawPitchRoll["y"].GetFloat(), luaYawPitchRoll["z"].GetFloat());
	// build the transform for the object
	Mat4x4 transform;
	transform.BuildYawPitchRoll(yawPitchRoll.x, yawPitchRoll.y, yawPitchRoll.z);
	transform.SetPosition(position);

	// create the object
	TiXmlElement* overloads = nullptr;
	StrongGameObjectPtr pObject = g_pApp->m_pGame->CreateGameObject(objectArchetype, overloads, &transform);

	// fire the new object created event
	if (pObject)
	{
		shared_ptr<Event_NewGameObject> pNewObjectEvent(CB_NEW Event_NewGameObject(pObject->GetId));
		IEventManager::Get()->QueueEvent(pNewObjectEvent);
		return pObject->GetId();
	}

	return INVALID_GAMEOBJECT_ID;
}

// add a lua listener callback for an event type. this will still be inserted into the C++ event manager
// which will fire events that are forward to the lua callback
unsigned long LuaInternalScriptExports::RegisterEventListener(EventType eventType, LuaPlus::LuaObject callbackFunction)
{
	CB_ASSERT(s_pScriptEventListenerMgr);
	// make sure the lua object is a function
	if (callbackFunction.IsFunction())
	{
		// create C++ proxy listener to listen for the event. this will forward the events to the lua callback
		LuaScriptEventListener* pListener = CB_NEW LuaScriptEventListener(eventType, callbackFunction);
		// add the listener to the lua listener manager
		s_pScriptEventListenerMgr->AddListener(pListener);
		// add its delegate listener function to the C++ event manager. this is where the events actually fire from
		IEventManager::Get()->AddListener(pListener->GetDelegate(), eventType);

		// conver the pointer to an unsigned long and use that as a handle
		unsigned long handle = reinterpret_cast<unsigned long>(pListener);
		return handle;
	}

	CB_ERROR("Invalid callback. Make sure the lua listener is a function");
	return 0;
}

// remove a lua event listener
void LuaInternalScriptExports::RemoveEventListener(unsigned long listenerId)
{
	CB_ASSERT(s_pScriptEventListenerMgr);
	CB_ASSERT(listenerId != 0);

	// convert the listener handle back to a pointer and destroy it
	LuaScriptEventListener* pListener = reinterpret_cast<LuaScriptEventListener*>(listenerId);
	s_pScriptEventListenerMgr->DestroyListener(pListener);
}

// queue an event from lua script
bool LuaInternalScriptExports::QueueEvent(EventType eventType, LuaPlus::LuaObject eventData)
{
	shared_ptr<LuaScriptEvent> pEvent(BuildEvent(eventType, eventData));
	if (pEvent)
	{
		IEventManager::Get()->QueueEvent(pEvent);
		return true;
	}

	return false;
}

// send an event immediately from lua script
bool LuaInternalScriptExports::TriggerEvent(EventType eventType, LuaPlus::LuaObject eventData)
{
	shared_ptr<LuaScriptEvent> pEvent(BuildEvent(eventType, eventData));
	if (pEvent)
	{
		IEventManager::Get()->TriggerEvent(pEvent);
		return true;
	}

	return false;
}

// attach a process from lua script
void LuaInternalScriptExports::AttachScriptProcess(LuaPlus::LuaObject scriptProcess)
{
	LuaPlus::LuaObject temp = scriptProcess.Lookup("__object");
	if (!temp.IsNil())
	{
		shared_ptr<Process> pProcess(static_cast<Process*>(temp.GetLightUserData()));
		g_pApp->m_pGame->AttachProcess(pProcess);
	}
	else
	{
		CB_ERROR("Could not find __object in script process");
	}
}

// get the y rotation in radians from a lua vec3
float LuaInternalScriptExports::GetYRotationFromVector(LuaPlus::LuaObject vec3)
{
	// make sure vec3 is a table
	if (vec3.IsTable())
	{
		Vec3 lookAt(vec3["x"].GetFloat(), vec3["y"].GetFloat(), vec3["z"].GetFloat());
		// use the GetYRotationFromVector from math.h
		return ::GetYRotationFromVector(lookAt);
	}

	CB_ERROR("Invalid object passed to GetYRotationFromVector(); type = " + std::string(vec3.TypeName()));
	return 0;
}

float LuaInternalScriptExports::WrapPi(float wrapMe)
{
	// use the WrapPi from math.h
	return ::WrapPi(wrapMe);
}

// create a lua vec3 table from an angle in radians
LuaPlus::LuaObject LuaInternalScriptExports::GetVectorFromRotation(float angleRadians)
{
	// use the GetVectorFromYRotation from math.h
	Vec3 result = ::GetVectorFromYRotation(angleRadians);

	// create a lua vec3 table from the result
	LuaPlus::LuaObject luaResult;
	luaResult.AssignNewTable(LuaStateManager::Get()->GetLuaState());
	luaResult.SetNumber("x", result.x);
	luaResult.SetNumber("y", result.y);
	luaResult.SetNumber("z", result.z);

	return luaResult;
}

// write to the log from lua script
void LuaInternalScriptExports::LuaLog(LuaPlus::LuaObject text)
{
	if (text.IsConvertibleToString())
	{
		CB_LOG("Lua", text.ToString());
	}
	else
	{
		CB_LOG("Lua", "<" + std::string(text.TypeName()) + ">");
	}
}

// get the tick count in lua
unsigned long LuaInternalScriptExports::GetTickCount()
{
	// use the system tick count
	return ::GetTickCount();
}

// apply force to an object from lua
void LuaInternalScriptExports::ApplyForce(LuaPlus::LuaObject normalDirectionLua, float force, int gameObjectId)
{
	if (normalDirectionLua.IsTable())
	{
		Vec3 normalDir(normalDirectionLua["x"].GetFloat(), normalDirectionLua["y"].GetFloat(), normalDirectionLua["z"].GetFloat());
		g_pApp->m_pGame->GetGamePhysics()->ApplyForce(normalDir, force, gameObjectId);
		return;
	}
	CB_ERROR("Invalid object passed to ApplyForce(). Type = " + std::string(normalDirectionLua.TypeName()));
}

// apply torque to an object from lua
void LuaInternalScriptExports::ApplyTorque(LuaPlus::LuaObject axisLua, float force, int gameObjectId)
{
	if (axisLua.IsTable())
	{
		Vec3 axis(axisLua["x"].GetFloat(), axisLua["y"].GetFloat(), axisLua["z"].GetFloat());
		g_pApp->m_pGame->GetGamePhysics()->ApplyTorque(axis, force, gameObjectId);
		return;
	}
	CB_ERROR("Invalid object passed to ApplyForce(). Type = " + std::string(axisLua.TypeName()));
}

// builds an event to be queued or triggered
shared_ptr<LuaScriptEvent> LuaInternalScriptExports::BuildEvent(EventType eventType, LuaPlus::LuaObject& eventData)
{
	// create the event from the event type
	shared_ptr<LuaScriptEvent> pEvent(LuaScriptEvent::CreateEventFromScript(eventType));
	if (!pEvent)
		return nullptr;
	
	// fill the event with event data
	if (!pEvent->SetEventData(eventData))
		return nullptr;

	return pEvent;
}

//=============================================================
//	C Functions for registering C++ functions to Lua script
//=============================================================
void LuaScriptExports::Register()
{
	LuaPlus::LuaObject globals = LuaStateManager::Get()->GetGlobalVars();

	// init
	LuaInternalScriptExports::Init();

	// resource loading
	globals.RegisterDirect("LoadAndExecuteScriptResource", &LuaInternalScriptExports::LoadAndExecuteScriptResource);

	// gameobjects
	globals.RegisterDirect("CreateObject", &LuaInternalScriptExports::CreateGameObject);

	// events
	globals.RegisterDirect("RegisterEventListener", &LuaInternalScriptExports::RegisterEventListener);
	globals.RegisterDirect("RemoveEventListener", &LuaInternalScriptExports::RemoveEventListener);
	globals.RegisterDirect("QueueEvent", &LuaInternalScriptExports::QueueEvent);
	globals.RegisterDirect("TriggerEvent", &LuaInternalScriptExports::TriggerEvent);

	// processes
	globals.RegisterDirect("AttachProcess", &LuaInternalScriptExports::AttachScriptProcess);

	// math (these are registered to GccMath, not global
	LuaPlus::LuaObject mathTable = globals.GetByName("GccMath");
	CB_ASSERT(mathTable.IsTable());
	mathTable.RegisterDirect("GetYRotationFromVector", &LuaInternalScriptExports::GetYRotationFromVector);
	mathTable.RegisterDirect("WrapPi", &LuaInternalScriptExports::WrapPi);
	mathTable.RegisterDirect("GetVectorFromRotation", &LuaInternalScriptExports::GetVectorFromRotation);

	// misc
	globals.RegisterDirect("Log", &LuaInternalScriptExports::LuaLog);
	globals.RegisterDirect("GetTickCount", &LuaInternalScriptExports::GetTickCount);

	// physics
	globals.RegisterDirect("ApplyForce", &LuaInternalScriptExports::ApplyForce);
	globals.RegisterDirect("ApplyTorque", &LuaInternalScriptExports::ApplyTorque);
}

void LuaScriptExports::Unregister()
{
	LuaInternalScriptExports::Destroy();
}