/*
	Events.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Events.h"

#include "EngineStd.h"
#include "Logger.h"
#include "PhysicsEvents.h"

//====================================================
//	Static Event GUID's
//====================================================
const EventType Event_UpdateTick::sk_EventType(0x53ff78f4);
const EventType Event_NewGameObject::sk_EventType(0xd020af46);
const EventType Event_DestroyGameObject::sk_EventType(0xd3b6f10e);
const EventType Event_MoveGameObject::sk_EventType(0x1a81ed5);
const EventType Event_NewRenderComponent::sk_EventType(0xfb30a10c);
const EventType Event_ModifiedRenderComponent::sk_EventType(0xf5ef297b);
const EventType Event_RequestNewGameObject::sk_EventType(0xe32a1a9a);
const EventType Event_RequestDestroyGameObject::sk_EventType(0xdc8c485d);
const EventType Event_EnvironmentLoaded::sk_EventType(0x8f28edab);
const EventType Event_RequestStartGame::sk_EventType(0xc46b8535);
const EventType Event_PlaySound::sk_EventType(0x366cce8e);


//====================================================
//	Event_NewGameObject
//====================================================
Event_NewGameObject::Event_NewGameObject()
{
	m_ObjectId = INVALID_GAMEOBJECT_ID;
	m_ViewId = CB_INVALID_GAMEVIEW_ID;
}

Event_NewGameObject::Event_NewGameObject(GameObjectId objectId, GameViewId viewId) :
m_ObjectId(objectId),
m_ViewId(viewId)
{ }

const GameObjectId Event_NewGameObject::GetObjectId() const
{
	return m_ObjectId;
}

const GameViewId Event_NewGameObject::GetViewId() const
{
	return m_ViewId;
}

const EventType& Event_NewGameObject::GetEventType() const
{
	return sk_EventType;
}

void Event_NewGameObject::Serialize(std::ostream& out) const
{
	out << m_ObjectId << " ";
	out << m_ViewId << " ";
}

void Event_NewGameObject::Deserialize(std::istream& in)
{
	in >> m_ObjectId;
	in >> m_ViewId;
}

IEventPtr Event_NewGameObject::Copy() const
{
	return IEventPtr(CB_NEW Event_NewGameObject(m_ObjectId, m_ViewId));
}

const char* Event_NewGameObject::GetName() const
{
	return "Event_NewGameObject";
}


//====================================================
//	Event_DestroyGameObject
//====================================================
Event_DestroyGameObject::Event_DestroyGameObject(GameObjectId id) :
m_Id(id)
{ }

const GameObjectId Event_DestroyGameObject::GetId() const
{
	return m_Id;
}

const EventType& Event_DestroyGameObject::GetEventType() const
{
	return sk_EventType;
}

void Event_DestroyGameObject::Serialize(std::ostream& out) const
{
	out << m_Id;
}

void Event_DestroyGameObject::Deserialize(std::istream& in)
{
	in >> m_Id;
}

IEventPtr Event_DestroyGameObject::Copy() const
{
	return IEventPtr(CB_NEW Event_DestroyGameObject(m_Id));
}

const char* Event_DestroyGameObject::GetName() const
{
	return "Event_DestroyGameObject";
}


//====================================================
//	Event_MoveGameObject
//====================================================
Event_MoveGameObject::Event_MoveGameObject()
{
	m_ObjectId = INVALID_GAMEOBJECT_ID;
}

Event_MoveGameObject::Event_MoveGameObject(GameObjectId id, const Mat4x4& matrix)
{
	m_ObjectId = id;
	m_Matrix = matrix;
}

const GameObjectId Event_MoveGameObject::GetId() const
{
	return m_ObjectId;
}

const Mat4x4& Event_MoveGameObject::GetMatrix() const
{
	return m_Matrix;
}

const EventType& Event_MoveGameObject::GetEventType() const
{
	return sk_EventType;
}

void Event_MoveGameObject::Serialize(std::ostream& out) const
{
	out << m_ObjectId << " ";
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			out << m_Matrix.m[i][j] << " ";
		}
	}
}

void Event_MoveGameObject::Deserialize(std::istream& in)
{
	in >> m_ObjectId;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			in >> m_Matrix.m[i][j];
		}
	}
}

IEventPtr Event_MoveGameObject::Copy() const
{
	return IEventPtr(CB_NEW Event_MoveGameObject(m_ObjectId, m_Matrix));
}

const char* Event_MoveGameObject::GetName() const
{
	return "Event_MoveGameObject";
}


//====================================================
//	Event_NewRenderComponent
//====================================================
Event_NewRenderComponent::Event_NewRenderComponent()
{
	m_ObjectId = INVALID_GAMEOBJECT_ID;
}

Event_NewRenderComponent::Event_NewRenderComponent(GameObjectId id, shared_ptr<SceneNode> pSceneNode)
{
	m_ObjectId = id;
	m_pSceneNode = pSceneNode;
}

const GameObjectId Event_NewRenderComponent::GetId() const
{
	return m_ObjectId;
}

shared_ptr<SceneNode> Event_NewRenderComponent::GetSceneNode() const
{
	return m_pSceneNode;
}

const EventType& Event_NewRenderComponent::GetEventType() const
{
	return sk_EventType;
}

void Event_NewRenderComponent::Serialize(std::ostream& out) const
{
	CB_ERROR(GetName() + std::string(" should not be serialized!"));
}

void Event_NewRenderComponent::Deserialize(std::istream& in)
{
	CB_ERROR(GetName() + std::string(" should not be deserialized!"));
}

IEventPtr Event_NewRenderComponent::Copy() const
{
	return IEventPtr(CB_NEW Event_NewRenderComponent(m_ObjectId, m_pSceneNode));
}

const char* Event_NewRenderComponent::GetName() const
{
	return "Event_NewRenderComponent";
}


//====================================================
//	Event_ModifiedRenderComponent
//====================================================
Event_ModifiedRenderComponent::Event_ModifiedRenderComponent()
{
	m_ObjectId = INVALID_GAMEOBJECT_ID;
}

Event_ModifiedRenderComponent::Event_ModifiedRenderComponent(GameObjectId id)
{
	m_ObjectId = id;
}

const GameObjectId Event_ModifiedRenderComponent::GetId() const
{
	return m_ObjectId;
}

const EventType& Event_ModifiedRenderComponent::GetEventType() const
{
	return sk_EventType;
}

void Event_ModifiedRenderComponent::Serialize(std::ostream& out) const
{
	out << m_ObjectId;
}

void Event_ModifiedRenderComponent::Deserialize(std::istream& in)
{
	in >> m_ObjectId;
}

IEventPtr Event_ModifiedRenderComponent::Copy() const
{
	return IEventPtr(CB_NEW Event_ModifiedRenderComponent(m_ObjectId));
}

const char* Event_ModifiedRenderComponent::GetName() const
{
	return "Event_ModifiedRenderComponent";
}


//====================================================
//	Event_RequestDestroyGameObject
//====================================================
Event_RequestNewGameObject::Event_RequestNewGameObject()
{
	m_ObjectResource = "";
	m_HasInitialTransform = false;
	m_InitialTransform = Mat4x4::Identity;
	m_ServerObjectId = -1;
	m_ViewId = CB_INVALID_GAMEVIEW_ID;
}

Event_RequestNewGameObject::Event_RequestNewGameObject(const std::string& objectResource, const Mat4x4* initialTransform, const GameObjectId serverObjectId, const GameViewId viewId)
{
	m_ObjectResource = objectResource;
	if (initialTransform)
	{
		m_HasInitialTransform = true;
		m_InitialTransform = *initialTransform;
	}
	else
	{
		m_HasInitialTransform = false;
	}

	m_ServerObjectId = serverObjectId;
	m_ViewId = viewId;
}

const std::string& Event_RequestNewGameObject::GetObjectResource() const
{
	return m_ObjectResource;
}

const GameViewId Event_RequestNewGameObject::GetViewId() const
{
	return m_ViewId;
}

const Mat4x4* Event_RequestNewGameObject::GetInitialTransform() const
{
	return (m_HasInitialTransform) ? &m_InitialTransform : nullptr;
}

const GameObjectId Event_RequestNewGameObject::GetServerObjectId() const
{
	return m_ServerObjectId;
}

const EventType& Event_RequestNewGameObject::GetEventType() const
{
	return sk_EventType;
}

void Event_RequestNewGameObject::Serialize(std::ostream& out) const
{
	out << m_ObjectResource << " ";
	out << m_HasInitialTransform << " ";
	if (m_HasInitialTransform)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				out << m_InitialTransform.m[i][j] << " ";
			}
		}
	}
	out << m_ServerObjectId << " ";
	out << m_ViewId << " ";
}

void Event_RequestNewGameObject::Deserialize(std::istream& in)
{
	in >> m_ObjectResource;
	in >> m_HasInitialTransform;
	if (m_HasInitialTransform)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				in >> m_InitialTransform.m[i][j];
			}
		}
	}
	in >> m_ServerObjectId;
	in >> m_ViewId;
}

IEventPtr Event_RequestNewGameObject::Copy() const
{
	return IEventPtr(CB_NEW Event_RequestNewGameObject(m_ObjectResource, (m_HasInitialTransform) ? &m_InitialTransform : nullptr, m_ServerObjectId, m_ViewId));
}

const char* Event_RequestNewGameObject::GetName() const
{
	return "Event_RequestNewGameObject";
}


//====================================================
//	Event_RequestDestroyGameObject
//====================================================
Event_RequestDestroyGameObject::Event_RequestDestroyGameObject()
{
	m_ObjectId = INVALID_GAMEOBJECT_ID;
}

Event_RequestDestroyGameObject::Event_RequestDestroyGameObject(GameObjectId id)
{
	m_ObjectId = id;
}

const GameObjectId Event_RequestDestroyGameObject::GetId() const
{
	return m_ObjectId;
}

const EventType& Event_RequestDestroyGameObject::GetEventType() const
{
	return sk_EventType;
}

void Event_RequestDestroyGameObject::Serialize(std::ostream& out) const
{
	out << m_ObjectId;
}

void Event_RequestDestroyGameObject::Deserialize(std::istream& in)
{
	in >> m_ObjectId;
}

IEventPtr Event_RequestDestroyGameObject::Copy() const
{
	return IEventPtr(CB_NEW Event_RequestDestroyGameObject(m_ObjectId));
}

const char* Event_RequestDestroyGameObject::GetName() const
{
	return "Event_RequestDestroyGameObject";
}

bool Event_RequestDestroyGameObject::BuildEventFromSCript()
{
	if (m_Event.IsInteger())
	{
		m_ObjectId = m_Event.GetInteger();
		return true;
	}
	return false;
}


// register the script events
void RegisterEngineScriptEvents()
{
	REGISTER_SCRIPT_EVENT(Event_RequestDestroyGameObject, Event_RequestDestroyGameObject::sk_EventType);
	REGISTER_SCRIPT_EVENT(Event_PhysCollision, Event_PhysCollision::sk_EventType);
	REGISTER_SCRIPT_EVENT(Event_PlaySound, Event_PlaySound::sk_EventType);
}
