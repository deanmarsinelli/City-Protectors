/*
	EventManager.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <list>
#include <map>

#include "interfaces.h"

const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

/**
	Manages events for the game. This class is a global singleton responsible for mapping
	event types to listeners.
*/
class EventManager : public IEventManager
{
	typedef std::list<EventListenerDelegate> EventListenerList;
	typedef std::map<EventType, EventListenerList> EventListenerMap;
	typedef std::list<IEventPtr> EventQueue;

public:
	/// Constructor to set the event manager global or not
	EventManager(const char* pName, bool setAsGlobal);

	/// Virtual destructor
	virtual ~EventManager();

	/// Registers a delegate function that will get called when the event type is triggered -- returns true if successful
	virtual bool AddListener(const EventListenerDelegate& eventDelegate, const EventType& type) = 0;

	/// Remove a delegate/event type pairing -- returns false if the pairing is not found
	virtual bool RemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type) = 0;

	/// Fire this event now and immediately call all delegate functions listening for this event
	virtual bool TriggerEvent(const IEventPtr& pEvent) const = 0;

	/// Queue the event and fire it on the next update if there is enough time
	virtual bool QueueEvent(const IEventPtr& pEvent) = 0;

	/// [thread safe] Queue the event and fire it on the next update if there is enough time
	virtual bool ThreadSafeQueueEvent(const IEventPtr& pEvent) = 0;

	/// Find the next instance of the event type and remove it from the processing queue -- if allOfType is true all events of this type will be removed
	virtual bool AbortEvent(const EventType& type, bool allOfType = false) = 0;

	/// Process events from the queue and optionally limit the processing time
	virtual bool Update(unsigned long maxMillis = kINFINITE) = 0;

private:
	/// Map from event types to lists of listeners for that type
	EventListenerMap m_EventListeners;

	/// Event queues for processing queued events
	EventQueue m_Queues[EVENTMANAGER_NUM_QUEUES];

	/// Which queue being actively processed
	int m_ActiveQueue;

	/// Thread safe event queue
	ThreadSafeEventQueue m_RealTimeEventQueue;
};
