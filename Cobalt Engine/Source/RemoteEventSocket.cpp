/*
	RemoteEventSocket.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <strstream>

#include "BinaryPacket.h"
#include "EngineStd.h"
#include "EventManager.h"
#include "interfaces.h"
#include "Logger.h"
#include "NetworkEvents.h"
#include "RemoteEventSocket.h"
#include "TextPacket.h"

RemoteEventSocket::RemoteEventSocket(SOCKET new_sock, unsigned int hostIP) :
NetSocket(new_sock, hostIP)
{ }

void RemoteEventSocket::HandleInput()
{
	NetSocket::HandleInput();

	// iterate the list of packets and handle them
	while (!m_InList.empty())
	{
		// get the first packet in the list
		shared_ptr<IPacket> packet = *m_InList.begin();
		m_InList.pop_front();

		// if the packet is a binary packet
		if (!strcmp(packet->GetType(), BinaryPacket::g_Type))
		{
			const char* buf = packet->GetData();
			int size = static_cast<int>(packet->GetSize());

			std::istrstream in(buf + sizeof(u_long), (size - sizeof(u_long)));

			int type;
			in >> type;

			// determine the type of event
			switch (type)
			{
			case NetMsg_Event:
				CreateEvent(in);
				break;

			case NetMsg_PlayerLoginOk:
			{
				int serverSockId, gameObjectId;
				in >> serverSockId;
				in >> gameObjectId;
				in >> g_pApp->m_Options.m_Level;
				shared_ptr<Event_NetworkPlayerObjectAssignment> pEvent(CB_NEW Event_NetworkPlayerObjectAssignment(gameObjectId, serverSockId));
				IEventManager::Get()->QueueEvent(pEvent);
				break;
			}

			default:
				CB_ERROR("Unknown message type in packet.");
			}
		}
		else if (!strcmp(packet->GetType(), TextPacket::g_Type))
		{
			CB_LOG("Network", packet->GetData() + sizeof(u_long));
		}
	}
}

void RemoteEventSocket::CreateEvent(std::istream& in)
{
	// create an event and dispatch it to the event manager
	EventType eventType;
	in >> eventType;

	IEventPtr pEvent(CREATE_EVENT(eventType));
	if (pEvent)
	{
		// deserialize the event from the network and dispatch it
		pEvent->Deserialize(in);
		IEventManager::Get()->QueueEvent(pEvent);
	}
	else
	{
		CB_ERROR("Error: Unknown event type from remote");
	}
}
