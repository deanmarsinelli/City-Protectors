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
#include "LuaStateManager.h"
#include "Matrix.h"
#include "ResourceCache.h"
#include "Vector.h"

/*
	This class is a C++ listener for script event listeners. It pairs an event type
	with a Lua callback function for that event. The event can be defined in C++ or in 
	Lua, and can be sent from C++ or Lua. 

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
		shared_ptr<LuaScriptEvent> pScriptEvent = static_pointer_cast<LuaScriptEvent>(pEventPtr);
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
	static void ApplyForce(LuaPlus::LuaObject normalDirection, float force, int gameObjectId);
	static void ApplyTorque(LuaPlus::LuaObject axis, float force, int gameObjectId);

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