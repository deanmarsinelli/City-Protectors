/*
	TextPacket.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "TextPacket.h"

const char* TextPacket::g_Type = "TextPacket";

TextPacket::TextPacket(const char* text) :
BinaryPacket(static_cast<u_long>(strlen(text) + 2))
{
	MemCpy(text, strlen(text), 0);

	// a carriage return and newline are added so text packets can be
	// parsed when coming in from a remote location
	MemCpy("\r\n", 2, 2);

	// set the size to 0
	*(u_long*)m_Data = 0;
}

const char* TextPacket::GetType() const
{
	return g_Type;
}
