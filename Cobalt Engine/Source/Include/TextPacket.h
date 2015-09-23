/*
	TextPacket.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "BinaryPacket.h"

/**
	This packet takes a text string as its data.
*/
class TextPacket : public BinaryPacket
{
public:
	/// Construct a packet from a text string
	TextPacket(const char* text);

	/// Return the type of the packet
	virtual const char* GetType() const;

public:
	/// Type name of the text packet
	static const char* g_Type;
};
