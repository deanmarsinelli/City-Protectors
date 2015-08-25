/*
	EventManager.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "EventManager.h"

#include "EngineStd.h"
#include "Logger.h"
#include "StringUtil.h"

GenericObjectFactory<IEvent, EventType> g_eventFactory;

//====================================================
//	IEventManager definitions
//====================================================
static IEventManager* g_pEventMgr = NULL;

IEventManager::IEventManager(const char* name, bool setAsGlobal)
{
	if (setAsGlobal)
	{
		if (g_pEventMgr)
		{
			CB_ERROR("Attempting to create two global event managers. This will overwrite the old one");
			delete g_pEventMgr;
		}

		g_pEventMgr = this;
	}
}

IEventManager::~IEventManager()
{ 
	if (g_pEventMgr)
		g_pEventMgr = nullptr;
}

IEventManager* IEventManager::Get()
{
	CB_ASSERT(g_pEventMgr);
	return g_pEventMgr;
}

//====================================================
//	EventManager definitions
//====================================================
EventManager::EventManager(const char* pName, bool setAsGlobal) :
IEventManager(pName, setAsGlobal)
{
	m_ActiveQueue = 0;
}

EventManager::~EventManager()
{ }

bool EventManager::AddListener(const EventListenerDelegate& eventDelegate, const EventType& type)
{
	CB_LOG("Events", "Attempting to add delegate listener for event type: " + ToStr(type, 16));

	// get the correct list from the map
	EventListenerList& listeners = m_EventListeners[type];

	// make sure the listener is not already registered
	for (EventListenerList::iterator it = listeners.begin(); it != listeners.end(); ++it)
	{
		if (eventDelegate == (*it))
		{
			CB_WARNING("Attempting to register the same delegate twice for this event");
			return false;
		}
	}

	// add the listener to the list
	listeners.push_back(eventDelegate);
	CB_LOG("Events", "Successfully added delegate listener for event type: " + ToStr(type, 16));
	
	return true;
}

bool EventManager::RemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type)
{
	CB_LOG("Events", "Attempting to remove delegate listener from event type: " + ToStr(type, 16));
	bool success = false;

	// iterate the map looking this event type
	auto findIt = m_EventListeners.find(type);
	if (findIt != m_EventListeners.end())
	{
		// get the list of listeners for this event type
		EventListenerList& listeners = findIt->second;
		for (auto it = listeners.begin(); it != listeners.end(); ++it)
		{
			// if the delegate listener is found, remove it
			if (eventDelegate == (*it))
			{
				listeners.erase(it);
				CB_LOG("Events", "Successfully removed delegate listener from event type: " + ToStr(type, 16));
				success = true;
				// break because there cannot be duplicate delegates for an event type
				break;
			}
		}
	}

	return success;
}

bool EventManager::TriggerEvent(const IEventPtr& pEvent) const
{
	CB_LOG("Events", "Attempting to trigger event " + std::string(pEvent->GetName()));
	bool processed = false;

	// iterate the map looking for this event type
	auto findIt = m_EventListeners.find(pEvent->GetEventType());
	if (findIt != m_EventListeners.end())
	{
		// iterate the listener list and send the event to each listener
		const EventListenerList& listeners = findIt->second;
		for (EventListenerList::const_iterator it = listeners.begin(); it != listeners.end(); ++it)
		{
			EventListenerDelegate listener = *it;
			CB_LOG("Events", "Sending event " + std::string(pEvent->GetName()) + " to delegate listener."); 
			listener(pEvent);
			processed = true;
		}
	}

	return processed;
}

bool EventManager::QueueEvent(const IEventPtr& pEvent)
{
	CB_ASSERT(m_ActiveQueue >= 0);
	CB_ASSERT(m_ActiveQueue < EVENTMANAGER_NUM_QUEUES);

	if (!pEvent)
	{
		CB_ERROR("Invalid Event");
	}
	CB_LOG("Events", "Attempting to queue event: " + std::string(pEvent->GetName()));

	// make sure there are listeners for this event
	auto findIt = m_EventListeners.find(pEvent->GetEventType());
	if (findIt != m_EventListeners.end())
	{
		m_Queues[m_ActiveQueue].push_back(pEvent);
		CB_LOG("Events", "Successfully queued event: " + std::string(pEvent->GetName()));
		return true;
	}
	else
	{
		CB_LOG("Events", "No listeners for event: " + std::string(pEvent->GetName()));
		return false;
	}
}

bool EventManager::ThreadSafeQueueEvent(const IEventPtr& pEvent)
{
	m_RealTimeEventQueue.push(pEvent);
	return true;
}

bool EventManager::AbortEvent(const EventType& type, bool allOfType)
{
	CB_ASSERT(m_ActiveQueue >= 0);
	CB_ASSERT(m_ActiveQueue < EVENTMANAGER_NUM_QUEUES);

	bool success = false;
	// find the event type in the map
	EventListenerMap::iterator findIt = m_EventListeners.find(type);
	if (findIt != m_EventListeners.end())
	{
		// get the active event queue
		EventQueue& queue = m_Queues[m_ActiveQueue];
		auto it = queue.begin();
		while (it != queue.end())
		{
			auto thisIt = it;
			++it;
			// remove the first occurrence of this event type from the queue,
			// if allOfType is true, remove all occurrences
			if ((*thisIt)->GetEventType() == type)
			{
				queue.erase(thisIt);
				success = true;

				if (!allOfType)
					break;
			}
		}
	}

	return success;
}

bool EventManager::Update(unsigned long maxMillis)
{
	unsigned long currMS = GetTickCount();
	// set the max milliseconds to process events
	unsigned long maxMS = (maxMillis == IEventManager::kINFINITE) ? IEventManager::kINFINITE : currMS + maxMillis;

	// handle events from other threads
	IEventPtr pRealtimeEvent;
	while (m_RealTimeEventQueue.try_pop(pRealtimeEvent))
	{
		QueueEvent(pRealtimeEvent);
		currMS = GetTickCount();

		if (maxMillis != IEventManager::kINFINITE)
		{
			if (currMS >= maxMS)
			{
				CB_ERROR("Too many real time processes hitting the event manager");
			}
		}
	}

	// swap active queues and clear the new active after the swap
	int queueToProcess = m_ActiveQueue;
	m_ActiveQueue = (m_ActiveQueue + 1) % EVENTMANAGER_NUM_QUEUES;
	m_Queues[m_ActiveQueue].clear();

	CB_LOG("EventLoop", "Processing Event Queue " + ToStr(queueToProcess) + "; " + ToStr((unsigned long)m_Queues[queueToProcess].size()) + " events to process");

	// process the queue of events
	while (!m_Queues[queueToProcess].empty())
	{
		IEventPtr pEvent = m_Queues[queueToProcess].front();
		CB_LOG("EventLoop", "\t\tProcessing Event " + std::string(pEvent->GetName()));

		const EventType& eventType = pEvent->GetEventType();

		// find all the listeners registered for this event in the map
		auto findIt = m_EventListeners.find(eventType);
		if (findIt != m_EventListeners.end())
		{
			// get the list of listeners
			const EventListenerList& listeners = findIt->second;
			CB_LOG("Event Loop", "\t\tFound " + ToStr((unsigned long)listeners.size()) + " listeners");

			// call each listener for this event type
			for (auto it = listeners.begin(); it != listeners.end(); ++it)
			{
				EventListenerDelegate listener = *it;
				CB_LOG("EventLoop", "\t\tSending event " + std::string(pEvent->GetName()) + " to listener");
				listener(pEvent);
			}
		}

		// check to see if time ran out
		currMS = GetTickCount();
		if (maxMillis != IEventManager::kINFINITE && currMS >= maxMS)
		{
			CB_LOG("EventLoop", "Aborting event processing, time ran out");
			break;
		}
	}
	bool queueFlushed = m_Queues[queueToProcess].empty();
	// if we couldn't process all events, push remaining events on the new active queue
	if (!queueFlushed)
	{
		while (!m_Queues[queueToProcess].empty())
		{
			IEventPtr pEvent = m_Queues[queueToProcess].back();
			m_Queues[queueToProcess].pop_back();
			m_Queues[m_ActiveQueue].push_front(pEvent);
		}
	}

	return queueFlushed;
}
