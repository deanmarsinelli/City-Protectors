/*
	LuaScriptEvent.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <functional>
#include <LuaPlus.h>
#include <unordered_map>

#include "BaseEvent.h"

/**
	Base class for a lua script event. This class is intended to be subclassed by
	any event that can be sent or received by the script. These events can also be
	received by C++ listeners.
*/
class LuaScriptEvent : public BaseEvent
{
	typedef std::unordered_map<EventType, std::function<LuaScriptEvent*()>> CreationFunctions;

public:
	/// Default constructor
	LuaScriptEvent() { m_EventIsValid = false; }

	/// Called when an event is sent from C++ to script
	LuaPlus::LuaObject GetEventData();

	/// Called when an event is sent from script to C++
	bool SetEventData(LuaPlus::LuaObject eventData);
	
	// Static helper functions for registering events with the script
	// Use REGISTER_SCRIPT_EVENT macro instead of calling these functions directly
	static void RegisterEventTypeWithScript(const char* key, EventType type);
	/// Map an event creation function with the event type
	static void AddCreationFunction(EventType type, std::function<LuaScriptEvent*()> creationFunction);
	static LuaScriptEvent* CreateEventFromScript(EventType type);

protected:
	// This function must be overridden if you want to fire this event from C++ and have it received by the script
	virtual void BuildEvent();

	// This function must be overridden if you want to fire this event from Script and have it received by C++
	virtual bool BuildEventFromScript() { return true; }

protected:
	/// Lua event object
	LuaPlus::LuaObject m_Event;

private:
	/// Map that maps event types to their creation functions
	static CreationFunctions s_CreationFunctions;
	
	/// Whether m_Event is valid or not
	bool m_EventIsValid;
};

// Macros for exporting events to script
#define REGISTER_SCRIPT_EVENT(eventClass, eventType) \
	LuaScriptEvent::RegisterEventTypeWithScript(#eventClass, eventType); \
	LuaScriptEvent::AddCreationFunction(eventType, &eventClass::CreateEventForScript)

#define EXPORT_FOR_SCRIPT_EVENT(eventClass) \
	public: \
	static LuaScriptEvent* CreateEventForScript() \
	{ \
		return new eventClass; \
	}
