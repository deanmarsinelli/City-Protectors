/*
	ProtectorController.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <SceneNode.h>

#include "GameEvents.h"
#include "ProtectorController.h"

const float OBJECT_ACCELERATION = 6.5f * 8.0f;
const float OBJECT_ANGULAR_ACCELERATION = 22.0f;

ProtectorController::ProtectorController(shared_ptr<SceneNode> object) :
m_Object(object)
{
	ZeroMemory(m_Key, sizeof(m_Key));
}

void ProtectorController::OnUpdate(float deltaTime)
{
	//
}

bool ProtectorController::OnPointerMove(const Point& mousePos, const int radius)
{
	return true;
}

bool ProtectorController::OnPointerButtonDown(const Point& mousePos, const int radius, const std::string& buttonName)
{
	if (buttonName != "PointerLeft")
	{
		return false;
	}
	
	// if left click pressed, fire weapon
	const GameObjectId objectId = m_Object->Get()->ObjectId();
	CB_ASSERT(objectId != INVALID_GAMEOBJECT_ID && L"The controller isn't attached to a valid game object");
	shared_ptr<Event_FireWeapon> pFireEvent(CB_NEW Event_FireWeapon(objectId));
	IEventManager::Get()->QueueEvent(pFireEvent);

	return true;
}

bool ProtectorController::OnPointerButtonUp(const Point& mousePos, const int radius, const std::string& buttonName)
{
	return (buttonName == "PointerLeft");
}

bool ProtectorController::OnKeyDown(const BYTE c)
{
	// update the key table
	m_Key[c] = true;

	// send a thrust event if necessary
	if (c == 'W' || c == 'S')
	{
		const GameObjectId objectId = m_Object->Get()->ObjectId();
		shared_ptr<Event_StartThrust> pEvent(CB_NEW Event_StartThrust(objectId, (c == 'W' ? OBJECT_ACCELERATION : (-OBJECT_ACCELERATION))));
		IEventManager::Get()->QueueEvent(pEvent);
	}

	// send a steer event if necessary
	if (c == 'A' || c == 'D')
	{
		const GameObjectId objectId = m_Object->Get()->ObjectId();
		shared_ptr<Event_StartSteer> pEvent(CB_NEW Event_StartSteer(objectId, (c == 'D' ? OBJECT_ANGULAR_ACCELERATION : (-OBJECT_ANGULAR_ACCELERATION))));
		IEventManager::Get()->QueueEvent(pEvent);
	}

	return true;
}

bool ProtectorController::OnKeyUp(const BYTE c)
{
	// update the key table
	m_Key[c] = false;

	// send an end thrust event if necessary
	if (c == 'W' || c == 'S')
	{
		const GameObjectId objectId = m_Object->Get()->ObjectId();
		shared_ptr<Event_EndThrust> pEvent(CB_NEW Event_EndThrust(objectId));
		IEventManager::Get()->QueueEvent(pEvent);
	}

	// send an end steer event if necessary
	if (c == 'A' || c == 'D')
	{
		const GameObjectId objectId = m_Object->Get()->ObjectId();
		shared_ptr<Event_EndSteer> pEvent(CB_NEW Event_EndSteer(objectId));
		IEventManager::Get()->QueueEvent(pEvent);
	}

	return true;
}
