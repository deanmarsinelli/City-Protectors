/*
	EventManager.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <list>
#include <unordered_map>

#include "interfaces.h"
#include "templates.h"

// Multiple Queues are used so that listener delegate functions can queue up more 
// events in the event queue without causing an endless loop of queueing
const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

/**
	Manages events for the game. This class is a global singleton responsible for mapping
	event types to listeners.
*/
class EventManager : public IEventManager
{
	typedef std::list<EventListenerDelegate> EventListenerList;
	typedef std::unordered_map<EventType, EventListenerList> EventListenerMap;
	typedef std::list<IEventPtr> EventQueue;

public:
	/// Constructor to set the event manager global or not
	EventManager(const char* pName, bool setAsGlobal);

	/// Virtual destructor
	virtual ~EventManager();

	/// Registers a delegate function that will get called when the event type is triggered -- returns true if successful
	virtual bool AddListener(const EventListenerDelegate& eventDelegate, const EventType& type);

	/// Remove a delegate/event type pairing -- returns false if the pairing is not found
	virtual bool RemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type);

	/// Fire this event now and immediately call all delegate functions listening for this event
	virtual bool TriggerEvent(const IEventPtr& pEvent) const;

	/// Queue the event and fire it on the next update if there is enough time
	virtual bool QueueEvent(const IEventPtr& pEvent);

	/// [thread safe] Queue the event and fire it on the next update if there is enough time
	virtual bool ThreadSafeQueueEvent(const IEventPtr& pEvent);

	/// Find the next instance of the event type and remove it from the processing queue -- if allOfType is true all events of this type will be removed
	virtual bool AbortEvent(const EventType& type, bool allOfType = false);

	/// Process events from the queue and optionally limit the processing time
	virtual bool Update(unsigned long maxMillis = kINFINITE);

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


/// Global event factory
extern GenericObjectFactory<IEvent, EventType> g_EventFactory;

// Register an event in the factory with the GUID as its key
#define REGISTER_EVENT(eventClass) g_EventFactory.Register<eventClass>(eventClass::sk_EventType)
#define CREATE_EVENT(eventType) g_EventFactory.Create(eventType)
