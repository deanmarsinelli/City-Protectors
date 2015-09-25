/*
	GameServerListenSocket.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "NetListenSocket.h"

class GameServerListenSocket : public NetListenSocket
{
public:
	/// Constructor taking in a port number to listen on
	GameServerListenSocket(int portNum);

	/// Handle an incoming connection
	virtual void HandleInput();
};
