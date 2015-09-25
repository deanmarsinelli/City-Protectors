/*
	NetworkEvents.h

	This file contains common events that are sent
	from client to server or server to client.

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "BaseEvent.h"
#include "EngineStd.h"

/**
	This event is sent by the server to the clients when a network view is 
	assigned a player number.
*/
class Event_NetworkPlayerObjectAssignment : public BaseEvent
{
public:
	/// Default constructor
	Event_NetworkPlayerObjectAssignment()
	{
		m_ObjectId = INVALID_GAMEOBJECT_ID;
		m_SocketId = -1;
	}

	/// Event with the object id and socket id
	explicit Event_NetworkPlayerObjectAssignment(const GameObjectId objectId, const int socketId) :
		m_ObjectId(objectId), m_SocketId(socketId)
	{ }

	/// Return the id of the object
	const GameObjectId GetGameObjectId() const
	{
		return m_ObjectId;
	}

	/// Return the id of the socket
	const int GetSocketId() const
	{
		return m_SocketId;
	}

	// IEvent interface
	/// Return the event type
	virtual const EventType& GetEventType() const
	{
		return sk_EventType;
	}

	/// Serialize the event to an output stream
	virtual void Serialize(std::ostream& out) const
	{
		out << m_ObjectId << " ";
		out << m_SocketId;
	}

	/// Deserialize the event from an input stream
	virtual void Deserialize(std::istream& in)
	{
		in >> m_ObjectId;
		in >> m_SocketId;
	}

	/// Return a copy of the event
	virtual IEventPtr Copy() const
	{
		return IEventPtr(CB_NEW Event_NetworkPlayerObjectAssignment(m_ObjectId, m_SocketId));
	}

	/// Return the name of the event
	virtual const char* GetName() const
	{
		return "Event_NetworkPlayerObjectAssignment";
	}

public:
	/// The event type
	static const EventType sk_EventType;

private:
	/// Id of the game object
	GameObjectId m_ObjectId;

	/// Id of the socket
	int m_SocketId;
};

/**
	This event is sent whenever a new client attaches to a server.
*/
class Event_RemoteClient : public BaseEvent
{
public:
	/// Default constructor
	Event_RemoteClient()
	{
		m_SocketId = 0;
		m_IpAddress = 0;
	}

	/// Event with the object id and socket id
	explicit Event_RemoteClient(const int socketId, const int ipAddress) :
		m_SocketId(socketId), m_IpAddress(ipAddress)
	{ }

	/// Return the id of the socket
	int GetSocketId() const
	{
		return m_SocketId;
	}

	/// Return the ip address of the remote connection
	int GetIpAddress() const
	{
		return m_IpAddress;
	}

	// IEvent interface
	/// Return the event type
	virtual const EventType& GetEventType() const
	{
		return sk_EventType;
	}

	/// Serialize the event to an output stream
	virtual void Serialize(std::ostream& out) const
	{
		out << m_SocketId << " ";
		out << m_IpAddress;
	}

	/// Deserialize the event from an input stream
	virtual void Deserialize(std::istream& in)
	{
		in >> m_SocketId;
		in >> m_IpAddress;
	}

	/// Return a copy of the event
	virtual IEventPtr Copy() const
	{
		return IEventPtr(CB_NEW Event_RemoteClient(m_SocketId, m_IpAddress));
	}

	/// Return the name of the event
	virtual const char* GetName() const
	{
		return "Event_RemoteClient";
	}

public:
	/// The event type
	static const EventType sk_EventType;

private:
	/// The id of the socket connection
	int m_SocketId;

	/// The ip of the the remote client
	int m_IpAddress;
};
