/*
	NetSocket.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <list>
#include <winsock.h>

#include "interfaces.h"

#define MAX_PACKET_SIZE (256)
#define RECV_BUFFER_SIZE (MAX_PACKET_SIZE * 512)


/**
	This class provides a basee class for a network socket connection.
*/
class NetSocket
{
	friend class BaseSocketManager;
	typedef std::list<shared_ptr<IPacket>> PacketList;

public:
	/// Default constructor used by clients before calling Connect
	NetSocket();

	/// Server constructor called when new clients connect
	NetSocket(SOCKET new_sock, unsigned int hostIP);

	/// Virtual destructor
	virtual ~NetSocket();

	/// Connect this socket to a remote host
	bool Connect(unsigned int ip, unsigned int port, bool forceCoalesce = 0);

	/// Set whether or not this is a blocking socket
	void SetBlocking(bool blocking);

	/// Send data through this socket
	void Send(shared_ptr<IPacket> packet, bool clearTimeout = 1);

	/// Return whether or not there are packets queued to be sent
	virtual int HasOutput();

	/// Send all packets that are queued for output
	virtual void HandleOutput();

	/// Read input into the input buffer
	virtual void HandleInput();

	virtual void Timeout();

	void HandleException();

	void SetTimeout(unsigned int ms = 45 * 1000);

	int GetIpAddress();

protected:
	/// The system socket
	SOCKET m_Sock;

	/// Unique id given by the socket manager
	int m_Id;

	/// Helps handle reconnections if the remote side drops
	int m_DeleteFlag;
	
	/// List of packets to be sent out 
	PacketList m_OutList;

	/// List of packets received
	PacketList m_InList;

	/// Receive buffer (holds the data just received)
	char m_RecvBuf[RECV_BUFFER_SIZE];

	/// Track the offset of the receive buffer head
	unsigned int m_RecvOffset;

	/// Track the beginning of the receive buffer
	unsigned int m_RecvBegin;

	bool m_BinaryProtocol;

	/// Offset for sending packets
	int m_SendOffset;

	/// When will the socket time out
	unsigned int m_TimeOut;

	/// The ip address of the remote connection
	unsigned int m_IpAddr;

	/// Is the remote IP internal?
	int m_Internal;

	/// Time the socket was created
	int m_TimeCreated;
};
