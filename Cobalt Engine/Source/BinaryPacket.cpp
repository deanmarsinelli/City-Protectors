/*
	BinaryPacket.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "BinaryPacket.h"
#include "EngineStd.h"
#include "Logger.h"

#pragma comment(lib, "Ws2_32")

const char* BinaryPacket::g_Type = "BinaryPacket";

BinaryPacket::BinaryPacket(const char* data, u_long size)
{
	// construct a packet all at once

	m_Data = CB_NEW char[size + sizeof(u_long)];
	CB_ASSERT(m_Data);


	// set the size of the data into the first 4 bytes (in network order)
	// from MSDN - The htonl function converts a u_long from host 
	// to TCP/IP network byte order (which is big-endian).
	*(u_long*)m_Data = htonl(size + sizeof(u_long));

	// copy the rest of the data after the size
	memcpy(m_Data + sizeof(u_long), data, size);
}

BinaryPacket::BinaryPacket(u_long size)
{
	// construct an empty packet

	m_Data = CB_NEW char[size + sizeof(u_long)];
	CB_ASSERT(m_Data);

	// set the size of the data into the first 4 bytes (in network order)
	*(u_long*)m_Data = htonl(size + sizeof(u_long));
}

BinaryPacket::~BinaryPacket()
{
	CB_SAFE_DELETE(m_Data);
}

void BinaryPacket::MemCpy(const char* data, size_t size, int destOffset)
{
	// copy data into the packet

	CB_ASSERT(size + destOffset <= GetSize() - sizeof(u_long));
	memcpy(m_Data + destOffset + sizeof(u_long), data, size);
}
	
const char* BinaryPacket::GetType() const
{
	return g_Type;
}

const char* BinaryPacket::GetData() const
{
	return m_Data;
}

u_long BinaryPacket::GetSize() const
{
	// return the size of the data, stored in the first 4 bytes
	// it must be converted from network order to host order

	// from MSDN - ntohl function converts a u_long from TCP/IP network order 
	// to host byte order (which is little-endian on Intel processors).
	return ntohl(*(u_long*)m_Data);
}
