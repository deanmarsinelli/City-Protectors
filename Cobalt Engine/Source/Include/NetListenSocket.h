/*
	NetListenSocket.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "NetSocket.h"

/**
	Implementation of a listen socket. This socket can listen for
	client connections on a specific port and accept them.
*/
class NetListenSocket : public NetSocket
{
public:
	/// Default constructor uses the default NetSocket constructor
	NetListenSocket() { };

	/// Construct a listen socket to listen on a specific port
	NetListenSocket(int portNum);

	/// Initialize the socket to listen on a specific port
	void Init(int portNum);

	/// Accept an incoming connection
	SOCKET AcceptConnection(unsigned int *pAddr);

public:
	/// Port that the socket is currently listening on
	unsigned short port;
};
