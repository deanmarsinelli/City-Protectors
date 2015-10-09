/*
	BaseSocketManager.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <list>
#include <string>
#include <unordered_map>

#include "NetSocket.h"

typedef std::list<NetSocket*> SocketList;
typedef std::unordered_map<int, NetSocket*> SocketIdMap;

/**
	This class organizes multiple sockets into a manageable group and takes care 
	of handling the initialization and shutdown of the sockets system. It provides
	a base class for more specialized socket managers for servers and clients. Each
	socket handle managed here will have a unique id and be stored in a hash map.
*/
class BaseSocketManager
{
public:
	/// Default constructor
	BaseSocketManager();

	/// Virtual destructor shuts down the manager
	virtual ~BaseSocketManager();

	/// This method sets up which sockets are going to be polled for activity,
	/// calls the select() API, handles processing of any socket with input, output,
	/// or exceptions, and closes any sockets that need closing
	void DoSelect(int pauseMicroSecs, bool handleInput = true);

	/// Initialize the OS socket system to allow connections
	bool Init();

	/// Destroy any managed sockets and shut down the OS socket system
	void Shutdown();

	/// Send human readable network errors to the error log
	void PrintError();

	/// Add a socket handle to the socket manager. It will be given the next id
	int AddSocket(NetSocket* socket);

	/// Remove a socket from the socket manager and close the socket
	void RemoveSocket(NetSocket* socket);

	/// Wraps gethostbyname() to return host information corresponding to a host name
	unsigned int GetHostByName(const std::string& hostName);

	/// Wraps gethostbyaddr() to return host information corresponding to a network address
	const char* GetHostByAddr(unsigned int ip);

	/// Return the ip address that specific socket is connected to
	int GetIpAddress(int sockId);

	/// Set the subnet for the socket manager
	void SetSubnet(unsigned int subnet, unsigned int subnetMask);

	/// Returns whether or not a particular ip is coming from the internal network
	bool IsInternal(unsigned int ip);

	/// Send a packet through a specific socket given by its id
	bool Send(int sockId, shared_ptr<IPacket> packet);

	/// Add a data amount to the outbound data counter
	void AddToOutbound(int rc);

	/// Add a data amount to the inbound data counter
	void AddToInbound(int rc);

protected:
	/// Return a pointer to a socket handle given the id
	NetSocket* FindSocket(int socketId);

protected:
	/// Describes the sockets system implementation
	WSADATA m_WsaData;

	/// List of socket handles that are being managed
	SocketList m_SockList;

	/// Map of integer ids to socket handles
	SocketIdMap m_SockMap;

	/// A ticker for the next socket id
	int m_NextSocketId;

	/// How much data has come in
	unsigned int m_Inbound;

	/// How much data has gone out
	unsigned int m_Outbound;

	/// Max number of open sockets handled by this manager
	unsigned int m_MaxOpenSockets;

	/// The subnet mask of the internal network
	unsigned int m_SubnetMask;

	/// The subnet of the internal network
	unsigned int m_Subnet;
};

/// Global socket manager pointer
extern BaseSocketManager* g_pSocketManager;
