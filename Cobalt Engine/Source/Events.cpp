/*
	Events.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Events.h"

#include "EngineStd.h"

//====================================================
//	Static Event GUID's
//====================================================
const EventType Event_NewGameObject::sk_EventType(0xd020af46);
const EventType Event_DestroyGameObject::sk_EventType(0xd3b6f10e);


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

const GameObjectId Event_NewGameObject::GetGameObjectId() const
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
