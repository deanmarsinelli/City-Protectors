/*
	GameEvents.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "GameEvents.h"

const EventType Event_FireWeapon::sk_EventType(0xed26dd23);
const EventType Event_StartThrust::sk_EventType(0xe0ace520);
const EventType Event_EndThrust::sk_EventType(0x41c7c27b);
const EventType Event_StartSteer::sk_EventType(0xdfd08ae6);
const EventType Event_EndSteer::sk_EventType(0x1378f779);
const EventType Event_GameplayUIUpdate::sk_EventType(0x94d5d17d);
const EventType Event_SetControlledObject::sk_EventType(0x4cdd3e2e);


void RegisterGameScriptEvents()
{
	REGISTER_SCRIPT_EVENT(Event_FireWeapon, Event_FireWeapon::sk_EventType);
	REGISTER_SCRIPT_EVENT(Event_StartThrust, Event_StartThrust::sk_EventType);
	REGISTER_SCRIPT_EVENT(Event_EndThrust, Event_EndThrust::sk_EventType);
	REGISTER_SCRIPT_EVENT(Event_StartSteer, Event_StartSteer::sk_EventType);
	REGISTER_SCRIPT_EVENT(Event_EndSteer, Event_EndSteer::sk_EventType);
	REGISTER_SCRIPT_EVENT(Event_GameplayUIUpdate, Event_GameplayUIUpdate::sk_EventType);
	REGISTER_SCRIPT_EVENT(Event_SetControlledObject, Event_SetControlledObject::sk_EventType);
}
