/*
	BinaryPacket.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "interfaces.h"

/**
	Implementation of a network packet that stores binary data. The packet can 
	be constructed all at once or with calls to MemCpy. The size of the buffer is 
	stored in the first 4 bytes of data in network order (size header + data).
*/
class BinaryPacket : public IPacket
{
public:
	/// Construct a packet all at once with data and size of the buffer
	BinaryPacket(const char* data, u_long size);

	/// Construct a packet with a given buffer size
	BinaryPacket(u_long size);

	/// Virtual destructor frees the packet data
	virtual ~BinaryPacket();

	/// Copy data into this packet
	void MemCpy(const char* data, size_t size, int destOffset);

	// IPacket interface
	/// Return the type of packet
	virtual const char* GetType() const;

	/// Return the data stored in the packet
	virtual const char* GetData() const;

	/// Return the size of the data stored in the packet
	virtual unsigned long GetSize() const;

public:
	/// Type name for binary packets
	static const char* g_Type;

protected:
	/// Pointer the data stored in the packet
	char* m_Data;
};
