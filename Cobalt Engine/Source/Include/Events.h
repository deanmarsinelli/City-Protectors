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
#include "LuaScriptEvent.h"

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
	const GameObjectId GetObjectId() const;

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


/**
	This event is sent by a server asking client proxy logics to create
	new objects from their resources. It can be sent via script or code.
*/
class Event_RequestNewGameObject : public BaseEvent
{
public:
	/// Default constructor
	Event_RequestNewGameObject();

	/// Constructor filling out the event
	Event_RequestNewGameObject(const std::string& objectResource, const Mat4x4* initialTransform = nullptr,
		const GameObjectId serverObjectId = INVALID_GAMEOBJECT_ID, const GameViewId viewId = CB_INVALID_GAMEVIEW_ID);

	/// Return a string of the object resource
	const std::string& GetObjectResource() const;

	/// Return the id of the view
	const GameViewId GetViewId() const;

	// Return the initial transform of the object
	const Mat4x4* GetInitialTransform() const;

	// Get the server game object id
	const GameObjectId GetServerObjectId() const;

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
	std::string m_ObjectResource;
	bool m_HasInitialTransform;
	Mat4x4 m_InitialTransform;
	GameObjectId m_ServerObjectId;
	GameViewId m_ViewId;
};


/**
	This event is sent by any system requesting that the game logic destroy a game object.
*/
class Event_RequestDestroyGameObject : public LuaScriptEvent
{
public:
	/// Default constructor
	Event_RequestDestroyGameObject();

	/// Constructor taking in the id of the object to be destroyed
	Event_RequestDestroyGameObject(GameObjectId id);

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

	/// Build the event from lua script
	virtual bool BuildEventFromSCript();

	/// Export this event so it can be called by script
	EXPORT_FOR_SCRIPT_EVENT(Event_RequestDestroyGameObject);

public:
	/// The event type
	static const EventType sk_EventType;

private:
	/// The game object for this event
	GameObjectId m_ObjectId;
};


/**
	This event is sent when a new game is started.
*/
class Event_EnvironmentLoaded : public BaseEvent
{
public:
	/// Default constructor
	Event_EnvironmentLoaded() { }

	// IEvent interface
	/// Return the event type
	virtual const EventType& GetEventType() const
	{
		return sk_EventType;
	}

	/// Return a copy of the event
	virtual IEventPtr Copy() const
	{
		return IEventPtr(CB_NEW Event_EnvironmentLoaded());
	}

	/// Return the name of the event
	virtual const char* GetName() const
	{
		return "Event_EnvironmentLoaded";
	}

public:
	/// The event type
	static const EventType sk_EventType;
};


/**
	This event is sent by the authoritative game logic to all views so they will load a game level.
*/
class Event_RequestStartGame : public BaseEvent
{
public:
	/// Default constructor
	Event_RequestStartGame() { }

	// IEvent interface
	/// Return the event type
	virtual const EventType& GetEventType() const
	{
		return sk_EventType;
	}

	/// Return a copy of the event
	virtual IEventPtr Copy() const
	{
		return IEventPtr(CB_NEW Event_RequestStartGame());
	}

	/// Return the name of the event
	virtual const char* GetName() const
	{
		return "Event_RequestStartGame";
	}

public:
	/// The event type
	static const EventType sk_EventType;
public:
	static const EventType sk_EventType;
};

/**
	This event is sent by any system wishing for a HumanView to play a sound.
*/
class Event_PlaySound : public LuaScriptEvent
{
public:
	/// Default constructor
	Event_PlaySound() { }

	/// Constructor with a sound resource
	Event_PlaySound(const std::string& soundResource) :
		m_SoundResource(soundResource)
	{ }

	// IEvent interface
	/// Return the event type
	virtual const EventType& GetEventType() const
	{
		return sk_EventType;
	}

	/// Return a copy of the event
	virtual IEventPtr Copy() const
	{
		return IEventPtr(CB_NEW Event_PlaySound());
	}

	/// Serialize the event
	virtual void Serialize(std::ostream& out) const
	{
		out << m_SoundResource;
	}

	/// Deserialize the event
	virtual void Deserialize(std::istream& in)
	{
		in >> m_SoundResource;
	}

	/// Return the sound resource
	const std::string& GetResource() const
	{
		return m_SoundResource;
	}

	/// Return the name of the event
	virtual const char* GetName() const
	{
		return "Event_PlaySound";
	}

	/// Build a C++ event from script
	virtual bool BuildEventFromScript()
	{
		if (m_Event.IsString())
		{
			m_SoundResource = m_Event.GetString();
			return true;
		}

		return false;
	}

	EXPORT_FOR_SCRIPT_EVENT(Event_PlaySound);

public:
	/// Type of the event
	static const EventType sk_EventType;

private:
	/// The name of the sound resource that is being played
	std::string m_SoundResource;
};

/// Register script events from the engine
extern void RegisterEngineScriptEvents();
