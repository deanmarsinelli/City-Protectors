/*
	PhysicsEvents.h

	This file contains common events that are used 
	for the physics sim.

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "BaseEvent.h"
#include "EngineStd.h"
#include "LuaScriptEvent.h"
#include "LuaStateManager.h"
#include "Vector.h"

/**
	Event sent when a game physics trigger object is triggered
	by another game object.
*/
class Event_PhysTriggerEnter : public BaseEvent
{
public:
	Event_PhysTriggerEnter()
	{
		m_TriggerId = -1;
		m_OtherId = INVALID_GAMEOBJECT_ID;
	}

	explicit Event_PhysTriggerEnter(int triggerId, GameObjectId other) :
		m_TriggerId(triggerId),
		m_OtherId(other)
	{ }

	int GetTriggerId() const
	{
		return m_TriggerId;
	}

	GameObjectId GetOtherObjectId() const
	{
		return m_OtherId;
	}

	virtual const EventType& GetEventType() const
	{
		return sk_EventType;
	}

	virtual IEventPtr Copy() const
	{
		return IEventPtr(CB_NEW Event_PhysTriggerEnter(m_TriggerId, m_OtherId));
	}

	virtual const char* GetName() const
	{
		return "Event_PhysTriggerEnter";
	}

public:
	static const EventType sk_EventType;

private:
	int m_TriggerId;
	GameObjectId m_OtherId;
};


/**
	This event is sent when a gameobject leaves a trigger
	objects physics collider.
*/
class Event_PhysTriggerLeave : public BaseEvent
{
public:
	Event_PhysTriggerLeave()
	{
		m_TriggerId = -1;
		m_OtherId = INVALID_GAMEOBJECT_ID;
	}

	explicit Event_PhysTriggerLeave(int triggerId, GameObjectId other) :
		m_TriggerId(triggerId),
		m_OtherId(other)
	{ }

	int GetTriggerId() const
	{
		return m_TriggerId;
	}

	GameObjectId GetOtherObjectId() const
	{
		return m_OtherId;
	}

	virtual const EventType& GetEventType() const
	{
		return sk_EventType;
	}

	virtual IEventPtr Copy() const
	{
		return IEventPtr(CB_NEW Event_PhysTriggerLeave(m_TriggerId, m_OtherId));
	}

	virtual const char* GetName() const
	{
		return "Event_PhysTriggerLeave";
	}

public:
	static const EventType sk_EventType;

private:
	int m_TriggerId;
	GameObjectId m_OtherId;
};


/**
	This event is sent when two physics objects begin colliding.
*/
class Event_PhysCollision : public LuaScriptEvent
{
public:
	Event_PhysCollision()
	{
		m_ObjectA = INVALID_GAMEOBJECT_ID;
		m_ObjectB = INVALID_GAMEOBJECT_ID;
		m_SumNormalForce = Vec3(0.0f, 0.0f, 0.0f);
		m_SumFrictionForce = Vec3(0.0f, 0.0f, 0.0f);
	}

	explicit Event_PhysCollision(GameObjectId objA, GameObjectId objB, const Vec3& sumNormalForce, 
		const Vec3& sumFrictionForce, const Vec3List& collisionPoints) :
		m_ObjectA(objA),
		m_ObjectB(objB),
		m_SumNormalForce(sumNormalForce),
		m_SumFrictionForce(sumFrictionForce),
		m_CollisionPoints(collisionPoints)
	{ }

	GameObjectId GetObjectA() const
	{
		return m_ObjectA;
	}

	GameObjectId GetObjectB() const
	{
		return m_ObjectB;
	}

	const Vec3& GetSumNormalForce() const
	{
		return m_SumNormalForce;
	}

	const Vec3& GetSumFrictionForce() const
	{
		return m_SumFrictionForce;
	}

	const Vec3List& GetCollisionPoints() const
	{
		return m_CollisionPoints;
	}

	virtual const EventType& GetEventType() const
	{
		return sk_EventType;
	}

	virtual IEventPtr Copy() const
	{
		return IEventPtr(CB_NEW Event_PhysCollision(m_ObjectA, m_ObjectB, m_SumNormalForce, m_SumFrictionForce, m_CollisionPoints));
	}

	virtual const char* GetName() const
	{
		return "Event_PhysCollision";
	}

	virtual void BuildEvent()
	{
		m_Event.AssignNewTable(LuaStateManager::Get()->GetLuaState());
		m_Event.SetInteger("objectA", m_ObjectA);
		m_Event.SetInteger("objectB", m_ObjectB);
	}

	EXPORT_FOR_SCRIPT_EVENT(Event_PhysCollision);

public:
	static const EventType sk_EventType;

private:
	GameObjectId m_ObjectA;
	GameObjectId m_ObjectB;
	Vec3 m_SumNormalForce;
	Vec3 m_SumFrictionForce;
	Vec3List m_CollisionPoints;
};


/**
	This event is sent when two colliding physics object are no 
	longer colliding.
*/
class Event_PhysSeparation : public BaseEvent
{
	Event_PhysSeparation()
	{
		m_ObjectA = INVALID_GAMEOBJECT_ID;
		m_ObjectB = INVALID_GAMEOBJECT_ID;
	}

	explicit Event_PhysSeparation(GameObjectId objA, GameObjectId objB) :
		m_ObjectA(objA),
		m_ObjectB(objB)
	{ }

	GameObjectId GetObjectA() const
	{
		return m_ObjectA;
	}

	GameObjectId GetObjectB() const
	{
		return m_ObjectB;
	}

	virtual const EventType& GetEventType() const
	{
		return sk_EventType;
	}

	virtual IEventPtr Copy() const
	{
		return IEventPtr(CB_NEW Event_PhysSeparation(m_ObjectA, m_ObjectB));
	}

	virtual const char* GetName() const
	{
		return "Event_PhysSeparation";
	}

public:
	static const EventType sk_EventType;

private:
	GameObjectId m_ObjectA;
	GameObjectId m_ObjectB;
};