/*
	ClientSocketManager.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <string>

#include "BaseSocketManager.h"

class ClientSocketManager : public BaseSocketManager
{
public:
	ClientSocketManager(const std::string& hostName, unsigned int port);

	bool Connect();

private:
	std::string m_HostName;
	unsigned int m_Port;
};