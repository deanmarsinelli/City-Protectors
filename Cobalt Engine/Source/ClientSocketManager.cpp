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
	if (!BaseSocketManager::Init())
	{
		return false;
	}

	RemoteEventSocket* pSocket = CB_NEW RemoteEventSocket;

	if (!pSocket->Connect(GetHostByName(m_HostName), m_Port))
	{
		CB_SAFE_DELETE(pSocket);
		return false;
	}

	AddSocket(pSocket);
	return true;
}
