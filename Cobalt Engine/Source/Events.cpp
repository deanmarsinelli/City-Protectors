/*
	Events.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Events.h"

//====================================================
//	Event_DestroyGameObject
//====================================================
Event_DestroyGameObject::Event_DestroyGameObject(GameObjectId id) :
m_Id(id)
{ }

GameObjectId Event_DestroyGameObject::GetId() const
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