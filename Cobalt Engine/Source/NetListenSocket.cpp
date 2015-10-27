/*
	NetListenSocket.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Logger.h"
#include "NetListenSocket.h"

NetListenSocket::NetListenSocket(int portNum)
{
	port = 0;
	Init(portNum);
}

void NetListenSocket::Init(int portNum)
{
	sockaddr_in sa;
	int value = 1;

	// create the socket handle
	if ((m_Sock = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		CB_ASSERT("NetListenSocket error: Init failed to create socket handle");
	}

	// set socket options
	if (setsockopt(m_Sock, SOL_SOCKET, SO_REUSEADDR, (char*)&value, sizeof(value) == SOCKET_ERROR))
	{
		perror("NetListenSocket::Init: setsockopt");
		closesocket(m_Sock);
		m_Sock = INVALID_SOCKET;
		CB_ASSERT("NetListenSocket error: Init failed to set options");
	}

	ZeroMemory(&sa, sizeof(sa));
	// set up the socket to listen on a port using the local ip
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons(portNum);

	// bind socket to a listen port
	if (bind(m_Sock, (sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
	{
		perror("NetListenSocket::Init: bind");
		closesocket(m_Sock);
		m_Sock = INVALID_SOCKET;
		CB_ASSERT("NetListenSocket error: Init failed bind to port");
	}

	// set the socket to non-blocking
	SetBlocking(false);

	// begin listening on that port
	if (listen(m_Sock, 256) == SOCKET_ERROR)
	{
		closesocket(m_Sock);
		m_Sock = INVALID_SOCKET;
		CB_ASSERT("NetListenSocket error: Init failed to listen");
	}

	port = portNum;
}

SOCKET NetListenSocket::AcceptConnection(unsigned int *pAddr)
{
	SOCKET new_sock;
	sockaddr_in sock;
	int size = sizeof(sock);

	// accept the incoming connection 
	// accept() returns a handle for the socket on which the actual connection is made
	if ((new_sock = accept(m_Sock, (sockaddr*)&sock, &size)) == INVALID_SOCKET)
	{
		return INVALID_SOCKET;
	}

	// grab the ip of the incoming connection client and place it into the sockaddr struct
	if (getpeername(new_sock, (sockaddr*)&sock, &size) == SOCKET_ERROR)
	{
		closesocket(new_sock);
		return INVALID_SOCKET;
	}

	// fill out the address of the connection
	*pAddr = ntohl(sock.sin_addr.s_addr);

	// return a handle to the socket that is the actual connection
	return new_sock;
}
