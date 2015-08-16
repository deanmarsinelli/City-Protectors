/*
	LuaScriptEvent.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "LuaScriptEvent.h"

#include "Logger.h"
#include "LuaStateManager.h"

LuaScriptEvent::CreationFunctions LuaScriptEvent::s_CreationFunctions;

LuaPlus::LuaObject LuaScriptEvent::GetEventData()
{
	// build the event if necessary and return it
	if (!m_EventIsValid)
	{
		BuildEvent();
		m_EventIsValid = true;
	}

	return m_Event;
}

bool LuaScriptEvent::SetEventData(LuaPlus::LuaObject eventData)
{
	// set the C++ event from the script
	m_Event = eventData;
	m_EventIsValid = BuildEventFromScript();
	return m_EventIsValid;
}

void LuaScriptEvent::RegisterEventTypeWithScript(const char* key, EventType type)
{
	// called to register an event type with the script to link them

	// get or create the EventType table
	LuaPlus::LuaObject eventTypeTable = LuaStateManager::Get()->GetGlobalVars().GetByName("EventType");
	if (eventTypeTable.IsNil())
		eventTypeTable = LuaStateManager::Get()->GetGlobalVars().CreateTable("EventType");

	CB_ASSERT(eventTypeTable.IsTable());
	CB_ASSERT(eventTypeTable[key].IsNil());

	// add the entry into the table
	eventTypeTable.SetInteger(key, type);
}

void LuaScriptEvent::AddCreationFunction(EventType type, std::function<LuaScriptEvent*()> creationFunction)
{
	CB_ASSERT(s_CreationFunctions.find(type) == s_CreationFunctions.end());
	CB_ASSERT(creationFunction != nullptr);
	s_CreationFunctions.insert(std::make_pair(type, creationFunction));
}

LuaScriptEvent* LuaScriptEvent::CreateEventFromScript(EventType type)
{
	// look up the event creation function for this type and return that function
	auto findIt = s_CreationFunctions.find(type);
	if (findIt != s_CreationFunctions.end())
	{
		auto func = findIt->second;
		return func();
	}
	else
	{
		CB_ERROR("Couldn't find the event type");
		return nullptr;
	}
}

void LuaScriptEvent::BuildEvent()
{
	// default behavior sets the data to nil
	m_Event.AssignNil(LuaStateManager::Get()->GetLuaState());
}
