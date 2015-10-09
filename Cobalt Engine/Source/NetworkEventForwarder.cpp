/*
	NetworkEventForwarder.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <strstream>

#include "BaseSocketManager.h"
#include "BinaryPacket.h"
#include "EngineStd.h"
#include "NetworkEventForwarder.h"
#include "RemoteEventSocket.h"

NetworkEventForwarder::NetworkEventForwarder(int socketId)
{
	m_SocketId = socketId;
}

void NetworkEventForwarder::ForwardEvent(IEventPtr pEvent)
{
	// this method serializes an event into a stream represented by
	// event message id-- event itself -- event type, and sends the 
	// stream to a specific socket

	std::ostrstream out;

	// serialze the event into an output stream
	out << static_cast<int>(RemoteEventSocket::NetMsg_Event) << " ";
	out << pEvent->GetEventType() << " ";
	pEvent->Serialize(out);
	out << "\r\n";

	shared_ptr<BinaryPacket> eventMsg(CB_NEW BinaryPacket(out.rdbuf()->str(), (u_long)out.pcount()));

	// send the event across the network
	g_pSocketManager->Send(m_SocketId, eventMsg);
}
