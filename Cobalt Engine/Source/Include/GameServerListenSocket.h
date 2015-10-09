/*
	GameServerListenSocket.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "NetListenSocket.h"

/**
	This class extends the listen socket class and is meant to be 
	a game server listen socket. It will overload the HandleInput() 
	method to create new clients encapsulated by RemoteEventSocket
	objects. It will send game events generated on the server to a 
	remote client and the client will receive them thinking they were
	generated locally.
*/
class GameServerListenSocket : public NetListenSocket
{
public:
	/// Constructor taking in a port number to listen on
	GameServerListenSocket(int portNum);

	/// Handle an incoming connection
	virtual void HandleInput();
};
