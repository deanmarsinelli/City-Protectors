/*
	Events.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "BaseEvent.h"

/**
	This event is sent when a game object is destroyed.
*/
class Event_DestroyGameObject : public BaseEvent
{
public:
	/// Constructor taking in the id of the object being destroyed
	explicit Event_DestroyGameObject(GameObjectId id = INVALID_GAMEOBJECT_ID);

	/// Return the Id of the object being destroyed
	GameObjectId GetId() const;

	// IEvent interface
	/// Return the event type
	virtual const EventType& GetEventType() const;

	/// Serialize the event to an output stream
	virtual void Serialize(std::ostream& out) const;

	/// Deserialize the event from an input stream
	virtual void Deserialize(std::istream& in);

	/// Return a copy of the event
	virtual IEventPtr Copy() const;

	/// Return the name of the event
	virtual const char* GetName() const;

public:
	/// The event type
	static const EventType sk_EventType;

private:
	/// The id of the object being destroyeds
	GameObjectId m_Id;
};
