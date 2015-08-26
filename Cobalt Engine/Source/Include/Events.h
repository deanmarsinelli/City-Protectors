/*
	Events.h

	This file contains common events that the engine
	supports by default.

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "BaseEvent.h"
#include "GameObject.h"
#include "HumanView.h"

/**
	This event is sent when a game object is created.
*/
class Event_NewGameObject : public BaseEvent
{
public:
	/// Default constructor
	Event_NewGameObject();

	/// Event with the object id and view id the object was paired with
	explicit Event_NewGameObject(GameObjectId objectId, GameViewId viewId = CB_INVALID_GAMEVIEW_ID);

	/// Return the id of the object that was created
	const GameObjectId GetGameObjectId() const;

	/// Return the id of the view
	const GameViewId GetViewId() const;

	// IEvent interface
	/// Return the event type
	virtual const EventType& GetEventType() const;

	/// Serialize the event to an output stream
	virtual void Serialize(std::ostream& out) const;

	/// Deserialize the event from an input stream
	virtual void Deserialize(std::istream& in);

	/// Return a copy of the event
	virtual IEventPtr Copy() const;

	/// Return the name of the event
	virtual const char* GetName() const;

public:
	/// The event type
	static const EventType sk_EventType;

private:
	/// Id of the object being created
	GameObjectId m_ObjectId;

	/// Id of the game view the object was paired with
	GameViewId m_ViewId;
};


/**
	This event is sent when a game object is destroyed.
*/
class Event_DestroyGameObject : public BaseEvent
{
public:
	/// Constructor taking in the id of the object being destroyed
	explicit Event_DestroyGameObject(GameObjectId id = INVALID_GAMEOBJECT_ID);

	/// Return the Id of the object being destroyed
	const GameObjectId GetId() const;

	// IEvent interface
	/// Return the event type
	virtual const EventType& GetEventType() const;

	/// Serialize the event to an output stream
	virtual void Serialize(std::ostream& out) const;

	/// Deserialize the event from an input stream
	virtual void Deserialize(std::istream& in);

	/// Return a copy of the event
	virtual IEventPtr Copy() const;

	/// Return the name of the event
	virtual const char* GetName() const;

public:
	/// The event type
	static const EventType sk_EventType;

private:
	/// The id of the object being destroyeds
	GameObjectId m_Id;
};


/**
	This event is sent when an object is moved.
*/
class Event_MoveGameObject : public BaseEvent
{
public:
	/// Default Constructor
	Event_MoveGameObject();

	/// Constructor taking in the id and transformation of the object being moved
	Event_MoveGameObject(GameObjectId id, const Mat4x4& matrix);

	/// Return the id of the object being moved
	const GameObjectId GetId() const;

	/// Return the tranformation
	const Mat4x4& GetMatrix() const;

	// IEvent interface
	/// Return the event type
	virtual const EventType& GetEventType() const;

	/// Serialize the event to an output stream
	virtual void Serialize(std::ostream& out) const;

	/// Deserialize the event from an input stream
	virtual void Deserialize(std::istream& in);

	/// Return a copy of the event
	virtual IEventPtr Copy() const;

	/// Return the name of the event
	virtual const char* GetName() const;

public:
	/// The event type
	static const EventType sk_EventType;

private:
	/// Id of the moving object
	GameObjectId m_ObjectId;

	/// Matrix holding the transformation
	Mat4x4 m_Matrix;
};


/**
	This event is sent when a game object is actually created. 
*/
class Event_NewRenderComponent : public BaseEvent
{
public:
	/// Default constructor
	Event_NewRenderComponent();

	/// Constructor taking in the id of an object and its scene node 
	explicit Event_NewRenderComponent(GameObjectId id, shared_ptr<SceneNode> pSceneNode);

	/// Return the id of the object being created
	const GameObjectId GetId() const;

	/// Return the scene node of the object being created
	shared_ptr<SceneNode> GetSceneNode() const;

	// IEvent interface
	/// Return the event type
	virtual const EventType& GetEventType() const;

	/// Serialize the event to an output stream
	virtual void Serialize(std::ostream& out) const;

	/// Deserialize the event from an input stream
	virtual void Deserialize(std::istream& in);

	/// Return a copy of the event
	virtual IEventPtr Copy() const;

	/// Return the name of the event
	virtual const char* GetName() const;

public:
	/// The event type
	static const EventType sk_EventType;

private:
	/// The game object for this event
	GameObjectId m_ObjectId;

	/// The scene node associated with the object
	shared_ptr<SceneNode> m_pSceneNode;
};


/**
	This event is sent when a render component is changed.
*/
class Event_ModifiedRenderComponent : public BaseEvent
{
public:
	/// Default constructor
	Event_ModifiedRenderComponent();

	/// Constructor taking in the id of the object being modified
	Event_ModifiedRenderComponent(GameObjectId id);

	/// Return the id of the object being modified
	const GameObjectId GetId() const;

	// IEvent interface
	/// Return the event type
	virtual const EventType& GetEventType() const;

	/// Serialize the event to an output stream
	virtual void Serialize(std::ostream& out) const;

	/// Deserialize the event from an input stream
	virtual void Deserialize(std::istream& in);

	/// Return a copy of the event
	virtual IEventPtr Copy() const;

	/// Return the name of the event
	virtual const char* GetName() const;

public:
	/// The event type
	static const EventType sk_EventType;

private:
	/// The game object for this event
	GameObjectId m_ObjectId;
};