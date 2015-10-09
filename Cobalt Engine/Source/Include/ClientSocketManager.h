/*
	ClientSocketManager.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <string>

#include "BaseSocketManager.h"

/**
	This class extends the base socket manager to manage the client side 
	of a game. It creates a single socket that attaches to a known server.
*/
class ClientSocketManager : public BaseSocketManager
{
public:
	/// Constructor sets up the host name and port of the server to connect to
	ClientSocketManager(const std::string& hostName, unsigned int port);

	/// Create a single socket and connect to the remote server
	bool Connect();

private:
	/// Host name of the server this socket will connect to
	std::string m_HostName;

	/// Port of the server this socket will connect to
	unsigned int m_Port;
};
