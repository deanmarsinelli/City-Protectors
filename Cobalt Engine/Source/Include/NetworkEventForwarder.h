/*
	NetworkEventForwarder.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "interfaces.h"

class NetworkEventForwarder
{
public:
	/// Default constructor taking in a socket id
	NetworkEventForwarder(int socketId);

	/// Forward an event down the socket over TCP/IP
	void ForwardEvent(IEventPtr pEvent);

protected:
	/// Id of the socket connection
	int m_SocketId;
};
