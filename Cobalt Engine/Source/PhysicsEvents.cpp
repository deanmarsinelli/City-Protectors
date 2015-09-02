/*
	PhysicsEvents.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "PhysicsEvents.h"

const EventType Event_PhysTriggerEnter::sk_EventType(0x5a33f83e);
const EventType Event_PhysTriggerLeave::sk_EventType(0x63d948dc);
const EventType Event_PhysCollision::sk_EventType(0xfd1a31d7);
const EventType Event_PhysSeparation::sk_EventType(0x8ce1de39);