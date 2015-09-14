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
	MemCpy("\r\n", 2, 2);
	*(u_long*)m_Data = 0;
}

const char* TextPacket::GetType() const
{
	return g_Type;
}
