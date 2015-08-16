/*
	ScriptComponent.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "ScriptComponent.h"

#include "Logger.h"
#include "LuaStateManager.h"
#include "Math.h"
#include "PhysicsComponent.h"
#include "templates.h"
#include "Transform.h"

// This is the name of the metatable where all the function definitions exported to Lua will live
static const char* LUA_METATABLE_NAME = "LuaScriptComponentMetaTable";
const char* LuaScriptComponent::g_Name = "LuaScriptComponent";

LuaScriptComponent::LuaScriptComponent()
{
	m_ScriptObject.AssignNil(LuaStateManager::Get()->GetLuaState());
	m_ScriptDestructor.AssignNil(LuaStateManager::Get()->GetLuaState());
}

LuaScriptComponent::~LuaScriptComponent()
{
	// call the script destructor if there is one
	if (m_ScriptDestructor.IsFunction())
	{
		LuaPlus::LuaFunction<void> func(m_ScriptDestructor);
		func(m_ScriptObject);
	}

	// clear the script object
	m_ScriptObject.AssignNil(LuaStateManager::Get()->GetLuaState());

	// if we were given a path for this script object, set it to nil and then execute that statement
	if (!m_ScriptObjectName.empty())
	{
		m_ScriptObjectName += " = nil;";
		LuaStateManager::Get()->ExecuteString(m_ScriptObjectName.c_str());
	}
}

bool LuaScriptComponent::Init(TiXmlElement* pData)
{
	LuaStateManager* pStateManager = LuaStateManager::Get();
	CB_ASSERT(pStateManager);

	// load the <LuaScriptObject> tag and validate it
	TiXmlElement* pScriptObjectElement = pData->FirstChildElement("LuaScriptObject");
	if (!pScriptObjectElement)
	{
		CB_ERROR("No <LuaScriptObject> tag in XML");
		return true;
	}

	// read in attributes
	const char* temp = nullptr;
	temp = pScriptObjectElement->Attribute("var"); // name of the variable in lua
	if (temp)
		m_ScriptObjectName = temp;

	temp = pScriptObjectElement->Attribute("constructor");
	if (temp)
		m_ConstructorName = temp;

	temp = pScriptObjectElement->Attribute("destructor");
	if (temp)
		m_DestructorName = temp;

	// having a "var" attribute will export this object to that name in lua
	if (!m_ScriptObjectName.empty())
	{
		m_ScriptObject = pStateManager->CreatePath(m_ScriptObjectName.c_str());
		if (!m_ScriptObject.IsNil())
		{
			// create the object in lua
			CreateScriptObject();
		}
	}

	// if there was a constructor, bind the constructor callback to that function in lua
	if (!m_ConstructorName.empty())
	{
		m_ScriptConstructor = pStateManager->GetGlobalVars().Lookup(m_ConstructorName.c_str());
		if (m_ScriptConstructor.IsFunction())
		{
			// if no "var" was given, create the script object here
			if (m_ScriptObject.IsNil())
			{
				m_ScriptObject.AssignNewTable(pStateManager->GetLuaState());
				CreateScriptObject();
			}
		}
	}

	// if there was a destructor, bind the destructor callback to that function in lua
	if (!m_DestructorName.empty())
	{
		m_ScriptDestructor = pStateManager->GetGlobalVars().Lookup(m_DestructorName.c_str());
	}

	// read the <LuaScriptData> tag
	TiXmlElement* pScriptDataElement = pData->FirstChildElement("LuaScriptData");
	if (pScriptDataElement)
	{
		if (m_ScriptObject.IsNil())
		{
			CB_ERROR("Error, m_ScriptObject should not be nil");
			return false;
		}

		// set up key/value pairs in lua
		for (TiXmlAttribute* pAttribute = pScriptDataElement->FirstAttribute(); pAttribute != nullptr; pAttribute = pAttribute->Next())
		{
			m_ScriptObject.SetString(pAttribute->Name(), pAttribute->Value());
		}
	}

	return true;
}

void LuaScriptComponent::PostInit()
{
	// call the script constructor if one was given
	if (m_ScriptConstructor.IsFunction())
	{
		LuaPlus::LuaFunction<void> func(m_ScriptConstructor);
		func(m_ScriptObject);
	}
}

TiXmlElement* LuaScriptComponent::GenerateXml()
{
	// serialize this component to an xml element
	TiXmlElement* pBaseElement = CB_NEW TiXmlElement(GetName());
	TiXmlElement* pScriptObjectElement = CB_NEW TiXmlElement("LuaScriptObject");
	
	if (!m_ScriptObjectName.empty())
		pScriptObjectElement->SetAttribute("var", m_ScriptObjectName.c_str());
	if (!m_ConstructorName.empty())
		pScriptObjectElement->SetAttribute("constructor", m_ConstructorName.c_str());
	if (!m_DestructorName.empty())
		pScriptObjectElement->SetAttribute("destructor", m_DestructorName.c_str());
	
	pBaseElement->LinkEndChild(pScriptObjectElement);

	return pBaseElement;
}

void LuaScriptComponent::RegisterScriptFunctions()
{
	// create the meta table
	LuaPlus::LuaObject metaTableObj = LuaStateManager::Get()->GetGlobalVars().CreateTable(LUA_METATABLE_NAME);
	metaTableObj.SetObject("__index", metaTableObj);

	// bind functions to lua object
	metaTableObj.RegisterObjectDirect("GetObjectId", (LuaScriptComponent*)0, &LuaScriptComponent::GetObjectId);
	metaTableObj.RegisterObjectDirect("GetPos", (LuaScriptComponent*)0, &LuaScriptComponent::GetPos);
	metaTableObj.RegisterObjectDirect("SetPos", (LuaScriptComponent*)0, &LuaScriptComponent::SetPos);
	metaTableObj.RegisterObjectDirect("GetLookAt", (LuaScriptComponent*)0, &LuaScriptComponent::GetLookAt);
	metaTableObj.RegisterObjectDirect("GetYOrientationRadians", (LuaScriptComponent*)0, &LuaScriptComponent::GetYOrientationRadians);
	metaTableObj.RegisterObjectDirect("RotateY", (LuaScriptComponent*)0, &LuaScriptComponent::RotateY);
	metaTableObj.RegisterObjectDirect("Stop", (LuaScriptComponent*)0, &LuaScriptComponent::Stop);
	metaTableObj.RegisterObjectDirect("SetPosition", (LuaScriptComponent*)0, &LuaScriptComponent::SetPosition);
}

void LuaScriptComponent::UnregisterScriptFunctions()
{
	// nil out the meta table object in lua
	LuaPlus::LuaObject metaTableObj = LuaStateManager::Get()->GetGlobalVars().Lookup(LUA_METATABLE_NAME);
	if (!metaTableObj.IsNil())
		metaTableObj.AssignNil(LuaStateManager::Get()->GetLuaState());
}

void LuaScriptComponent::CreateScriptObject()
{
	LuaStateManager* pStateManager = LuaStateManager::Get();
	CB_ASSERT(pStateManager);
	CB_ASSERT(!m_ScriptObject.IsNil());

	LuaPlus::LuaObject metaTableObj = pStateManager->GetGlobalVars().Lookup(LUA_METATABLE_NAME);
	CB_ASSERT(!metaTableObj.IsNil());

	// bind the __object field in lua to this object
	LuaPlus::LuaObject boxedPtr = pStateManager->GetLuaState()->BoxPointer(this);
	boxedPtr.SetMetaTable(metaTableObj);
	m_ScriptObject.SetLightUserData("__object", this);
	m_ScriptObject.SetMetaTable(metaTableObj);
}

LuaPlus::LuaObject LuaScriptComponent::GetObjectId()
{
	// return the object id to lua
	LuaPlus::LuaObject ret;
	ret.AssignInteger(LuaStateManager::Get()->GetLuaState(), m_pOwner->GetId());
	return ret;
}

LuaPlus::LuaObject LuaScriptComponent::GetPos()
{
	// return the objects position to lua
	LuaPlus::LuaObject ret;
	Transform transform = m_pOwner->transform;
	LuaStateManager::Get()->ConvertVec3ToTable(transform.GetPosition(), ret);
	return ret;
}

void LuaScriptComponent::SetPos(LuaPlus::LuaObject newPos)
{
	// set the position of an object in lua
	Vec3 pos;
	LuaStateManager::Get()->ConvertTableToVec3(newPos, pos);
	m_pOwner->transform.SetPosition(pos);
}

LuaPlus::LuaObject LuaScriptComponent::GetLookAt() const
{
	// return the look at vector of an object to lua
	LuaPlus::LuaObject ret;
	Transform transform = m_pOwner->transform;
	LuaStateManager::Get()->ConvertVec3ToTable(transform.GetLookAt(), ret);
	return ret;
}

float LuaScriptComponent::GetYOrientationRadians() const
{
	// return the look at vector of an object to lua
	Transform transform = m_pOwner->transform;
	return GetYRotationFromVector(transform.GetLookAt());
}

void LuaScriptComponent::RotateY(float angleRadians)
{
	shared_ptr<PhysicsComponent> pPhysicalComponent = MakeStrongPtr(m_pOwner->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name))
	if (pPhysicalComponent)
		pPhysicalComponent->RotateY(angleRadians);
}

void LuaScriptComponent::SetPosition(float x, float y, float z)
{
	shared_ptr<PhysicsComponent> pPhysicalComponent = MakeStrongPtr(m_pOwner->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name))
	if (pPhysicalComponent)
		pPhysicalComponent->SetPosition(x, y, z);
}

void LuaScriptComponent::LookAt(Vec3 target)
{

}

void LuaScriptComponent::Stop()
{
	shared_ptr<PhysicsComponent> pPhysicalComponent = MakeStrongPtr(m_pOwner->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name))
	if (pPhysicalComponent)
		pPhysicalComponent->Stop();
}
