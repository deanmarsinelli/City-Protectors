/*
	NetworkGameView.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <FastDelegate.h>
#include <strstream>

#include "BaseSocketManager.h"
#include "BinaryPacket.h"
#include "EventManager.h"
#include "Events.h"
#include "NetworkEvents.h"
#include "NetworkGameView.h"
#include "RemoteEventSocket.h"
#include "templates.h"

NetworkGameView::NetworkGameView()
{
	m_SockId = INVALID_SOCKET_ID;
	m_GameObjectId = INVALID_GAMEOBJECT_ID;

	// add this object as a listener for new game object events
	IEventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &NetworkGameView::NewGameObjectDelegate), Event_NewGameObject::sk_EventType);
}

void NetworkGameView::NewGameObjectDelegate(IEventPtr pEvent)
{
	// get a pointer to the game object
	shared_ptr<Event_NewGameObject> pCastEvent = static_pointer_cast<Event_NewGameObject>(pEvent);
	GameObjectId objectId = pCastEvent->GetGameObjectId();
	StrongGameObjectPtr pObject = MakeStrongPtr(g_pApp->m_pGame->GetGameObject(objectId));

	if (pObject && pObject->GetType() == "Teapot")
	{
		if (pCastEvent->GetViewId() == m_ViewId)
		{
			m_GameObjectId = objectId;
			shared_ptr<Event_NetworkPlayerObjectAssignment> pEvent(CB_NEW Event_NetworkPlayerObjectAssignment(m_GameObjectId, m_SockId));
			IEventManager::Get()->QueueEvent(pEvent);
		}
	}
}

void NetworkGameView::AttachRemotePlayer(int sockID)
{
	m_SockId = sockID;

	std::ostrstream out;
	out << static_cast<int>(RemoteEventSocket::NetMsg_PlayerLoginOk) << " ";
	out << m_SockId << " ";
	out << m_GameObjectId << " ";
	out << g_pApp->m_Options.m_Level << " ";
	out << "\r\n";

	shared_ptr<BinaryPacket> gvidMsg(CB_NEW BinaryPacket(out.rdbuf()->str(), (u_long)out.pcount()));
	g_pSocketManager->Send(m_SockId, gvidMsg);
}

void NetworkGameView::OnAttach(GameViewId viewId, GameObjectId objectId)
{
	m_ViewId = viewId;
	m_GameObjectId = objectId;
}

void NetworkGameView::OnUpdate(float deltaTime)
{
	if (m_GameObjectId != INVALID_GAMEOBJECT_ID)
	{
		IEventManager::Get()->RemoveListener(fastdelegate::MakeDelegate(this, &NetworkGameView::NewGameObjectDelegate), Event_NewGameObject::sk_EventType);
	}
}
