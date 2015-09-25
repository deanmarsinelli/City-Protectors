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


class BaseSocketManager
{
public:
	/// Default constructor
	BaseSocketManager();

	/// Virtual destructor shuts down the manager
	virtual ~BaseSocketManager();

	void DoSelect(int pauseMicroSecs, bool handleInput = true);

	bool Init();

	void Shutdown();

	/// Send human readable network errors to the error log
	void PrintError();

	int AddSocket(NetSocket* socket);

	void RemoveSocket(NetSocket* socket);

	unsigned int GetHostByName(const std::string& hostName);

	const char* GetHostByAddr(unsigned int ip);

	int GetIpAddress(int sockId);

	void SetSubnet(unsigned int subnet, unsigned int subnetMask);

	bool IsInternal(unsigned int ip);

	bool Send(int sockId, shared_ptr<IPacket> packet);

	void AddToOutbound(int rc);

	void AddToInbound(int rc);

protected:
	/// Return a pointer to a socket given the id
	NetSocket* FindSocket(int socketId);

protected:
	WSADATA m_WsaData;

	SocketList m_SockList;
	SocketIdMap m_SockMap;

	int m_NextSocketId;

	unsigned int m_Inbound;

	unsigned int m_Outbound;

	unsigned int m_MaxOpenSockets;

	unsigned int m_SubnetMask;

	unsigned int m_Subnet;
};

/// Global socket manager pointer
extern BaseSocketManager* g_pSocketManager;
