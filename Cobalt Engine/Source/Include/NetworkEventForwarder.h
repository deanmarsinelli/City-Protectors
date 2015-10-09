/*
	NetworkEventForwarder.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "interfaces.h"

/**
	This class is used to forward events down a network connection. It 
	stores a socket id which will be used to determine which socket
	to send the data to.
*/
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
