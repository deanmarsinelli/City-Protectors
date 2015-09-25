/*
	GameServerListenSocket.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "BaseSocketManager.h"
#include "GameServerListenSocket.h"
#include "NetworkEvents.h"
#include "RemoteEventSocket.h"

GameServerListenSocket::GameServerListenSocket(int portNum)
{
	// begin listening on this port
	Init(portNum);
}

void GameServerListenSocket::HandleInput()
{
	unsigned int ipAddr;
	// create a socket between the client and server
	SOCKET new_sock = AcceptConnection(&ipAddr);

	int value = 1;
	setsockopt(new_sock, SOL_SOCKET, SO_DONTLINGER, (char*)&value, sizeof(value));

	if (new_sock != INVALID_SOCKET)
	{
		RemoteEventSocket* sock = CB_NEW RemoteEventSocket(new_sock, ipAddr);
		int sockId = g_pSocketManager->AddSocket(sock);
		int ipAddress = g_pSocketManager->GetIpAddress(sockId);

		// dispatch an event that a remote client has connected
		shared_ptr<Event_RemoteClient> pEvent(CB_NEW Event_RemoteClient(sockId, ipAddress));
		IEventManager::Get()->QueueEvent(pEvent);
	}
}
