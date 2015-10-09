/*
	NetListenSocket.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "NetSocket.h"

/**
	Implementation of a listen socket and an extension to NetSocket. This socket 
	can listen for client connections on a specific port and accept them, adding
	new sockets (the actual connections) to the global socket manager. There are 
	5 steps to create a listen socket: create a socket handle, set the socket 
	options, bind the socket to a listen port, set it to non-blocking, and call 
	listen() -- these steps are all handled by Init().
*/
class NetListenSocket : public NetSocket
{
public:
	/// Default constructor uses the default NetSocket constructor
	NetListenSocket() { };

	/// Construct a listen socket to listen on a specific port
	NetListenSocket(int portNum);

	/// Initialize the socket to listen on a specific port (the 5 steps listed above)
	void Init(int portNum);

	/// Attach a client and create a new socket handle
	SOCKET AcceptConnection(unsigned int *pAddr);

public:
	/// Port that the socket is currently listening on
	unsigned short port;
};
