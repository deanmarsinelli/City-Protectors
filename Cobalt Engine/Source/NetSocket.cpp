/*
	NetSocket.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "BaseSocketManager.h"
#include "BinaryPacket.h"
#include "EngineStd.h"
#include "Logger.h"
#include "NetSocket.h"
#include "TextPacket.h"

#pragma comment(lib, "Ws2_32")

NetSocket::NetSocket()
{
	// client socket constructor
	m_Sock = INVALID_SOCKET;
	m_DeleteFlag = 0;
	m_SendOffset = 0;
	m_TimeOut = 0;
	m_RecvOffset = 0;
	m_RecvBegin = 0;
	m_Internal = 0;
	m_BinaryProtocol = 1;
}

NetSocket::NetSocket(SOCKET new_sock, unsigned int hostIP)
{
	// server socket constructor
	m_DeleteFlag = 0;
	m_SendOffset = 0;
	m_TimeOut = 0;
	m_RecvOffset = 0;
	m_RecvBegin = 0;
	m_Internal = 0;
	m_BinaryProtocol = 1;

	m_TimeCreated = timeGetTime();

	m_Sock = new_sock;
	m_IpAddr = hostIP;

	m_Internal = g_pSocketManager->IsInternal(m_IpAddr);

	setsockopt(m_Sock, SOL_SOCKET, SO_DONTLINGER, NULL, 0);
}

NetSocket::~NetSocket()
{
	if (m_Sock != INVALID_SOCKET)
	{
		closesocket(m_Sock);
		m_Sock = INVALID_SOCKET;
	}
}

bool NetSocket::Connect(unsigned int ip, unsigned int port, bool forceCoalesce)
{
	sockaddr_in sa;
	int x = 1;

	// create the socket
	if ((m_Sock = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		return false;
	}

	// by default, disable packet grouping and send them as fast as possible
	if (!forceCoalesce)
	{
		setsockopt(m_Sock, IPPROTO_TCP, TCP_NODELAY, (char*)&x, sizeof(x));
	}

	// fill out the sockaddr_in struct with the ip and port
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(ip);
	sa.sin_port = htons(port);

	// connect to the remote location
	if (connect(m_Sock, (sockaddr*)&sa, sizeof(sa)))
	{
		// if failed to connect, close the socket
		closesocket(m_Sock);
		m_Sock = INVALID_SOCKET;
		return false;
	}

	return true;
}

void NetSocket::SetBlocking(bool blocking)
{
	// set the socket to block or not
	unsigned long val = blocking ? 0 : 1;
	ioctlsocket(m_Sock, FIONBIO, &val);
}

void NetSocket::Send(shared_ptr<IPacket> packet, bool clearTimeout)
{
	if (clearTimeout)
	{
		m_TimeOut = 0;
	}
	// add the packet to the end of the list to be sent
	// packets are queued and sent once per update
	m_OutList.push_back(packet);
}

int NetSocket::HasOutput()
{
	return !m_OutList.empty();
}

void NetSocket::HandleOutput()
{
	// send all the packets queued to be sent through the socket

	int fSent = 0;
	do
	{
		CB_ASSERT(!m_OutList.empty());
		auto it = m_OutList.begin();

		shared_ptr<IPacket> packet = *it;
		// get the data and sizefrom the current packet
		const char* buf = packet->GetData();
		int len = static_cast<int>(packet->GetSize());

		// send the data through the socket, this uses an offset in case a packet 
		// is split and needs to be sent in multiple loop iterations
		int rc = send(m_Sock, buf + m_SendOffset, len - m_SendOffset, 0);
		if (rc > 0)
		{
			// rc contains number of bytes sent
			g_pSocketManager->AddToOutbound(rc);
			m_SendOffset += rc;
			fSent = 1;
		}
		else if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			// if there was an exception
			HandleException();
			fSent = 0;
		}
		else
		{
			// error or no bytes sent
			fSent = 0;
		}

		// once the entire packet has been sent, pop it from the queue
		// and reset the offset
		if (m_SendOffset == packet->GetSize())
		{
			m_OutList.pop_front();
			m_SendOffset = 0;
		}

	} while (fSent && !m_OutList.empty());
}

void NetSocket::HandleInput()
{
	// read in data from the socket and compose it into discrete packets that the game can read

	bool packetReceived = false;
	u_long packetSize = 0;

	// read the socket into the receiver buffer which acts as a ring buffer
	int rc = recv(m_Sock, m_RecvBuf + m_RecvBegin + m_RecvOffset, RECV_BUFFER_SIZE - (m_RecvBegin + m_RecvOffset), 0);

	if (rc == 0)
	{
		return;
	}
	
	if (rc == SOCKET_ERROR)
	{
		m_DeleteFlag = 1;
		return;
	}

	// header size = 4 bytes
	const int hdrSize = sizeof(u_long);
	// get the end of the new data read in
	unsigned int newData = m_RecvOffset + rc;
	int processedData = 0;

	while (newData > hdrSize)
	{
		// we support two types of packets:
		// Binary packet - size is a positive 4 byte int at the start of data
		// Text packet - size 0, parser searches for carriage return

		// convert the first 4 bytes received to host order - this is the size of this packet
		packetSize = *(reinterpret_cast<u_long*>(m_RecvBuf + m_RecvBegin));
		packetSize = ntohl(packetSize);

		if (m_BinaryProtocol)
		{
			// if the data is less than the packet size, then we haven't finished
			// reading this packet and dont have enough data to get the next packet
			if (newData < packetSize)
			{
				break;
			}
			
			if (packetSize > MAX_PACKET_SIZE)
			{
				// buffer overrun
				HandleException();
				return;
			}

			// we know the packet size and have the entire thing read in
			if (newData >= packetSize)
			{
				// create the packet from the receiver buffer (m_RecvBegin + hrdSize so we dont read the header *size* into the data)
				shared_ptr<BinaryPacket> packet(CB_NEW BinaryPacket(&m_RecvBuf[m_RecvBegin + hdrSize], packetSize - hdrSize));
				m_InList.push_back(packet);
				packetReceived = true;
				processedData += packetSize;
				newData -= packetSize;
				// move the receiver head up by the packet size amount
				m_RecvBegin += packetSize;
			}
		}
		else
		{
			// text protocol waits for a carriage return and creates a string
			// scan the memory we just read for this character
			char* cr = static_cast<char*>(memchr(&m_RecvBuf[m_RecvBegin], 0x0a, rc));
			if (cr)
			{
				// if found, change the '\n' in the text packet to a null terminator
				*(cr + 1) = 0;
				shared_ptr<TextPacket> packet(CB_NEW TextPacket(&m_RecvBuf[m_RecvBegin]));
				m_InList.push_back(packet);
				packetSize = cr - &m_RecvBuf[m_RecvBegin];
				packetReceived = true;
				processedData += packetSize;
				newData -= packetSize;
				// move the receiver head up by the packet size amount
				m_RecvBegin += packetSize;
			}
		}
	}

	g_pSocketManager->AddToInbound(rc);
	m_RecvOffset = newData;

	if (packetReceived)
	{
		if (m_RecvOffset == 0)
		{
			m_RecvBegin = 0;
		}
		else if (m_RecvBegin + m_RecvOffset + MAX_PACKET_SIZE > RECV_BUFFER_SIZE)
		{
			// dont want to allow data to overrun the receiver buffer, so copy leftover bits
			// to the beginning and reset the heads
			int leftover = m_RecvOffset;
			memcpy(m_RecvBuf, &m_RecvBuf[m_RecvBegin], m_RecvOffset);
			m_RecvBegin = 0;
		}
	}
}

void NetSocket::Timeout()
{
	m_TimeOut = 0;
}

void NetSocket::HandleException()
{
	// set the 2nd bit to on, close the socket and set to invalid socket
	// but to not delete the netsocket object
	m_DeleteFlag |= 1;
}

void NetSocket::SetTimeout(unsigned int ms)
{
	m_TimeOut = timeGetTime() + ms;
}

int NetSocket::GetIpAddress()
{
	return m_IpAddr;
}
