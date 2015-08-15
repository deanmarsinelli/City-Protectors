/*
	BaseEvent.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "interfaces.h"

/**
	This is the base class for all event data classes and should be subclassed.
*/
class BaseEvent : public IEvent
{
public:
	/// Constructor sets the time stamp of the event
	explicit BaseEvent(const float timeStamp = 0.0f) :
		m_TimeStamp(timeStamp) { }
	
	/// Return the type of the event
	virtual const EventType& GetEventType() const = 0;

	/// Return the time stamp of the event
	float GetTimeStamp() const { return m_TimeStamp; }

	/// Serialze the event to an output stream
	virtual void Serialize(std::ostream& out) const { }

	/// Deserialize an event from an input stream
	virtual void Deserialize(std::istream& in) { }

private:
	/// Time that the event occured
	const float m_TimeStamp;
};