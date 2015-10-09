/*
	RemoteEventSocket.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "NetSocket.h"

/**
	This class is used to convert network socket data into events that
	can be sent to the local game event system.
*/
class RemoteEventSocket : public NetSocket
{
public:
	enum
	{
		NetMsg_Event,
		NetMsg_PlayerLoginOk
	};

	/// Constructor taking in a socket and ip
	RemoteEventSocket(SOCKET new_sock, unsigned int hostIP);

	/// Default constructor to attach a client to a server
	RemoteEventSocket() { }

	/// Handle packets that have come in from the network
	virtual void HandleInput();

protected:
	/// Create an event from network data and dispatch the event
	void CreateEvent(std::istream& in);
};