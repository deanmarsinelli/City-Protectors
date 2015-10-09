/*
	ClientSocketManager.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "ClientSocketManager.h"
#include "EngineStd.h"
#include "RemoteEventSocket.h"

ClientSocketManager::ClientSocketManager(const std::string& hostName, unsigned int port)
{
	m_HostName = hostName;
	m_Port = port;
}

bool ClientSocketManager::Connect()
{
	// intialize the base class
	if (!BaseSocketManager::Init())
	{
		return false;
	}

	// create a single socket to handle event passing
	RemoteEventSocket* pSocket = CB_NEW RemoteEventSocket;

	// connect this socket to a remote server
	if (!pSocket->Connect(GetHostByName(m_HostName), m_Port))
	{
		CB_SAFE_DELETE(pSocket);
		return false;
	}

	// add this socket to the manager (list and map)
	AddSocket(pSocket);
	return true;
}
