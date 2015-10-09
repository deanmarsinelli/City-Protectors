/*
	BaseSocketManager.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "BaseSocketManager.h"
#include "EngineStd.h"
#include "Logger.h"

BaseSocketManager *g_pSocketManager = nullptr;

BaseSocketManager::BaseSocketManager()
{
	m_Inbound = 0;
	m_Outbound = 0;
	m_MaxOpenSockets = 0;
	m_SubnetMask = 0;
	m_Subnet = 0xffffffff;
	m_NextSocketId = 0;

	g_pSocketManager = this;
	ZeroMemory(&m_WsaData, sizeof(WSADATA));
}

BaseSocketManager::~BaseSocketManager()
{
	Shutdown();
}

void BaseSocketManager::DoSelect(int pauseMicroSecs, bool handleInput)
{
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = pauseMicroSecs; // microsec = 1 millionth of a second

	fd_set inp_set, out_set, exc_set;
	int maxdesc = 0;

	FD_ZERO(&inp_set);
	FD_ZERO(&out_set);
	FD_ZERO(&exc_set);

	// set everything up for the select, add sockets to the inp_set and out_set
	for (auto it = m_SockList.begin(); it != m_SockList.end(); ++it)
	{
		NetSocket* pSock = *it;
		if ((pSock->m_DeleteFlag & 1) || pSock->m_Sock == INVALID_SOCKET)
		{
			continue;
		}

		if (handleInput)
		{
			FD_SET(pSock->m_Sock, &inp_set);
		}

		FD_SET(pSock->m_Sock, &exc_set);

		if (pSock->HasOutput())
		{
			FD_SET(pSock->m_Sock, &out_set);
		}

		if ((int)pSock->m_Sock > maxdesc)
		{
			maxdesc = (int)pSock->m_Sock;
		}
	}

	int selRet = 0;

	// select will poll the sockets for input and output
	selRet = select(maxdesc + 1, &inp_set, &out_set, &exc_set, &tv);
	if (selRet = SOCKET_ERROR)
	{
		PrintError();
		return;
	}

	// handle input, output, and exceptions
	if (selRet)
	{
		for (auto it = m_SockList.begin(); it != m_SockList.end(); ++it)
		{
			NetSocket* pSock = *it;

			if ((pSock->m_DeleteFlag & 1) || pSock->m_Sock == INVALID_SOCKET)
			{
				continue;
			}

			if (FD_ISSET(pSock->m_Sock, &exc_set))
			{
				pSock->HandleException();
			}

			if (!(pSock->m_DeleteFlag & 1) && FD_ISSET(pSock->m_Sock, &out_set))
			{
				pSock->HandleOutput();
			}

			if (handleInput && !(pSock->m_DeleteFlag & 1) && FD_ISSET(pSock->m_Sock, &inp_set))
			{
				pSock->HandleInput();
			}
		}
	}

	unsigned int timeNow = timeGetTime();

	// handle deleting any sockets
	auto it = m_SockList.begin();
	while (it != m_SockList.end())
	{
		NetSocket* pSock = *it;
		if (pSock->m_TimeOut)
		{
			if (pSock->m_TimeOut < timeNow)
			{
				pSock->Timeout();
			}
		}

		if (pSock->m_DeleteFlag & 1)
		{
			switch (pSock->m_DeleteFlag)
			{
			// id a sockets delete flag is set to 1, remove it entirely from the manager and destroy the netsocket object
			case 1:
				g_pSocketManager->RemoveSocket(pSock);
				it = m_SockList.begin();
				break;

			case 3:
				// if the delete flag is set to 2, the socket handle is shut down but the netsocket object stays around for
				// potential reconnections. this allows unsent packets to be stored and sent when a client reconnects
				pSock->m_DeleteFlag = 2;
				if (pSock->m_Sock != INVALID_SOCKET)
				{
					closesocket(pSock->m_Sock);
					pSock->m_Sock = INVALID_SOCKET;
					break;
				}
			}
		}
		++it;
	}
}

bool BaseSocketManager::Init()
{
	if (WSAStartup(0x0202, &m_WsaData) == 0)
	{
		return true;
	}
	else
	{
		CB_ERROR("WSAStartup failed.");
		return false;
	}
}

void BaseSocketManager::Shutdown()
{
	// clear the socket list
	while (!m_SockList.empty())
	{
		delete *m_SockList.begin();
		m_SockList.pop_front();
	}

	WSACleanup();
}

void BaseSocketManager::PrintError()
{
	int realError = WSAGetLastError();
	char* reason;

	switch (realError)
	{
	case WSANOTINITIALISED: reason = "A successful WSAStartup must occur before using this API."; break;
	case WSAEFAULT: reason = "The Windows Sockets implementation was unable to allocated needed resources for its internal operations, or the readfds, writefds, exceptfds, or timeval parameters are not part of the user address space."; break;
	case WSAENETDOWN: reason = "The network subsystem has failed."; break;
	case WSAEINVAL: reason = "The timeout value is not valid, or all three descriptor parameters were NULL."; break;
	case WSAEINTR: reason = "The (blocking) call was canceled via WSACancelBlockingCall."; break;
	case WSAEINPROGRESS: reason = "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function."; break;
	case WSAENOTSOCK: reason = "One of the descriptor sets contains an entry which is not a socket."; break;
	default: reason = "Unknown.";
	}

	char buffer[256];
	sprintf(buffer, "Socket error: %s", reason);
	CB_LOG("Network", buffer);
}

int BaseSocketManager::AddSocket(NetSocket* socket)
{
	socket->m_Id = m_NextSocketId;
	m_SockMap[m_NextSocketId] = socket;
	m_NextSocketId++;

	m_SockList.push_front(socket);
	if (m_SockList.size() > m_MaxOpenSockets)
	{
		m_MaxOpenSockets++;
	}

	return socket->m_Id;
}

void BaseSocketManager::RemoveSocket(NetSocket* socket)
{
	m_SockList.remove(socket);
	m_SockMap.erase(socket->m_Id);
	CB_SAFE_DELETE(socket);
}

unsigned int BaseSocketManager::GetHostByName(const std::string& hostName)
{
	// retrieve the ip details into a host ent struct
	hostent* pHostEnt = gethostbyname(hostName.c_str());
	sockaddr_in tmpSockAddr;

	if (!pHostEnt)
	{
		CB_ASSERT(0 && L"Error Occured getting Host by name");
		return 0;
	}

	memcpy(&tmpSockAddr.sin_addr, pHostEnt->h_addr, pHostEnt->h_length);
	return ntohl(tmpSockAddr.sin_addr.s_addr);
}

const char* BaseSocketManager::GetHostByAddr(unsigned int ip)
{
	static char host[256];

	int netip = htonl(ip);
	hostent* lpHostEnt = gethostbyaddr((const char*)&netip, 4, PF_INET);

	if (lpHostEnt)
	{
		strcpy_s(host, 256, lpHostEnt->h_name);
		return host;
	}

	return nullptr;
}

int BaseSocketManager::GetIpAddress(int sockId)
{
	NetSocket* sock = FindSocket(sockId);
	if (sock)
	{
		return sock->GetIpAddress();
	}
	else
	{
		return 0;
	}
}

void BaseSocketManager::SetSubnet(unsigned int subnet, unsigned int subnetMask)
{
	m_Subnet = subnet;
	m_SubnetMask = subnetMask;
}

bool BaseSocketManager::IsInternal(unsigned int ip)
{
	if (!m_SubnetMask)
	{
		return false;
	}

	if ((ip & m_SubnetMask) == m_Subnet)
	{
		return false;
	}

	return true;
}

bool BaseSocketManager::Send(int sockId, shared_ptr<IPacket> packet)
{
	// find the specific socket by its id
	NetSocket* sock = FindSocket(sockId);
	if (!sock)
	{
		return false;
	}

	// send the packet to the socket
	sock->Send(packet);
	return true;
}

void BaseSocketManager::AddToOutbound(int rc)
{
	m_Outbound += rc;
}

void BaseSocketManager::AddToInbound(int rc)
{
	m_Inbound += rc;
}

NetSocket* BaseSocketManager::FindSocket(int socketId)
{
	// iterate the map of sockets and return a handle
	// to the correct socket given by the id
	auto it = m_SockMap.find(socketId);
	if (it != m_SockMap.end())
	{
		return it->second;
	}

	return nullptr;
}
