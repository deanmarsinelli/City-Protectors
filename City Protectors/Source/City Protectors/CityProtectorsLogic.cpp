/*
	CityProtectorsLogic.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <GameObject.h>
#include <Events.h>
#include <Logger.h>
#include <NetworkEvents.h>
#include <NetworkGameView.h>
#include <Physics.h>
#include <PhysicsComponent.h>
#include <PhysicsEvents.h>
#include <TransformComponent.h>

#include "AIView.h"
#include "CityProtectorsHumanView.h"
#include "CityProtectorsLogic.h"
#include "GameEvents.h"

CityProtectorsLogic::CityProtectorsLogic()
{
	// create the physics world and register all event delegates
	m_pPhysics.reset(CreateGamePhysics());
	RegisterAllDelegates();
}

CityProtectorsLogic::~CityProtectorsLogic()
{
	// remove event delegates
	RemoveAllDelegates();
	DestroyAllNetworkEventForwarders();
}

// overrides
void CityProtectorsLogic::MoveGameObject(const GameObjectId id, const Mat4x4& mat)
{
	BaseGameLogic::MoveGameObject(id, mat);

	// destroy any object that goes under the world
	StrongGameObjectPtr pObject = MakeStrongPtr(GetGameObject(id));
	if (pObject)
	{
		shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(pObject->GetComponent<TransformComponent>(TransformComponent::g_Name));
		if (pTransformComponent && pTransformComponent->GetPosition().y < -25)
		{
			shared_ptr<Event_DestroyGameObject> pDestroyObjectEvent(CB_NEW Event_DestroyGameObject(id));
			IEventManager::Get()->QueueEvent(pDestroyObjectEvent);
		}
	}
}

void CityProtectorsLogic::ChangeState(BaseGameState newState)
{
	// base class change state
	BaseGameLogic::ChangeState(newState);

	switch (newState)
	{
	case BaseGameState::WaitingForPlayers:
	{
		// only one local player allowed
		CB_ASSERT(m_ExpectedPlayers == 1);

		// add each local human players view
		for (int i = 0; i < m_ExpectedPlayers; i++)
		{
			shared_ptr<IGameView> playersView(CB_NEW CityProtectorsHumanView(g_pApp->m_Renderer));
			AddView(playersView);

			if (m_Proxy)
			{
				// if we are remote, we spawn our view and are done
				return;
			}
		}

		// spawn all remote player's views in the host logic
		for (int i = 0; i < m_ExpectedRemotePlayers; i++)
		{
			shared_ptr<IGameView> remoteGameVIew(CB_NEW NetworkGameView);
			AddView(remoteGameVIew);
		}

		// spawn all AI views in the host logic
		for (int i = 0; i < m_ExpectedAI; i++)
		{
			shared_ptr<IGameView> aiView(CB_NEW AIView(m_pPathingGraph));
			AddView(aiView);
		}

		break;
	}
	case BaseGameState::SpawningPlayersObjects:
	{
		if (m_Proxy)
		{
			// only the server needs to spawn player objects
			return;
		}

		for (auto it = m_GameViews.begin(); it != m_GameViews.end(); ++it)
		{
			shared_ptr<IGameView> pView = (*it);
			if (pView->GetType() == GameView_Human)
			{
				// create the game object and send an event
				StrongGameObjectPtr pObject = CreateGameObject("objects\\player_teapot.xml", nullptr);
				if (pObject)
				{
					shared_ptr<Event_NewGameObject> pEvent(CB_NEW Event_NewGameObject(pObject->GetId(), pView->GetId()));
					IEventManager::Get()->TriggerEvent(pEvent);
				}
			}
			else if (pView->GetType() == GameView_Remote)
			{
				// cast to remote view, create the object, and send an event
				shared_ptr<NetworkGameView> pNetworkGameView = static_pointer_cast<NetworkGameView, IGameView>(pView);
				StrongGameObjectPtr pObject = CreateGameObject("objects\\remote_teapot.xml", nullptr);
				if (pObject)
				{
					shared_ptr<Event_NewGameObject> pEvent(CB_NEW Event_NewGameObject(pObject->GetId(), pNetworkGameView->GetId()));
					IEventManager::Get()->QueueEvent(pEvent);
				}
			}
			else if (pView->GetType() == GameView_AI)
			{
				// cast to ai view, create the object, and send an event
				shared_ptr<AIView> pAiView = static_pointer_cast<AIView, IGameView>(pView);
				StrongGameObjectPtr pObject = CreateGameObject("objects\\ai_teapot.xml", nullptr);
				if (pObject)
				{
					shared_ptr<Event_NewGameObject> pEvent(CB_NEW Event_NewGameObject(pObject->GetId(), pAiView->GetId()));
					IEventManager::Get()->QueueEvent(pEvent);
				}
			}
		}
		break;
	}
	}
}

void CityProtectorsLogic::AddView(shared_ptr<IGameView> pView, GameObjectId objectId)
{
	BaseGameLogic::AddView(pView, objectId);

	// increment the correct counter if this is a human view or ai view
	if (dynamic_pointer_cast<CityProtectorsHumanView>(pView))
	{
		m_HumanPlayersAttached++;
	}
	else if (dynamic_pointer_cast<AIView>(pView))
	{
		m_AIPlayersAttached++;
	}
}

void CityProtectorsLogic::RequestStartGameDelegate(IEventPtr pEvent)
{
	ChangeState(BaseGameState::WaitingForPlayers);
}

void CityProtectorsLogic::RemoteClientDelegate(IEventPtr pEvent)
{
	// this event is sent from clients to the game server
	shared_ptr<Event_RemoteClient> pCastEvent = static_pointer_cast<Event_RemoteClient>(pEvent);
	const int sockID = pCastEvent->GetSocketId();
	const int ipAddress = pCastEvent->GetIpAddress();

	// find a NetworkGameView that doesn't have a socketID, and attach this client to that view
	for (auto it = m_GameViews.begin(); it != m_GameViews.end(); ++it)
	{
		shared_ptr<IGameView> pView = (*it);
		if (pView->GetType() == GameView_Remote)
		{
			// cast the view as a network view
			shared_ptr<NetworkGameView> pNetworkGameView = static_pointer_cast<NetworkGameView, IGameView>(pView);
			if (!pNetworkGameView->HasRemotePlayerAttached())
			{
				// attach the remote player to this view (on the server)
				pNetworkGameView->AttachRemotePlayer(sockID);
				CreateNetworkEventForwarder(sockID);
				m_HumanPlayersAttached++;

				return;
			}
		}
	}
}

void CityProtectorsLogic::NetworkPlayerObjectAssignmentDelegate(IEventPtr pEvent)
{
	// for remote players getting assigned to a game object

	if (!m_Proxy)
		return;

	shared_ptr<Event_NetworkPlayerObjectAssignment> pCastEvent = static_pointer_cast<Event_NetworkPlayerObjectAssignment>(pEvent);
	if (pCastEvent->GetObjectId() == INVALID_GAMEOBJECT_ID)
	{
		// if the remote player has no id, its id is now the socket id
		m_RemotePlayerId = pCastEvent->GetSocketId();
		return;
	}

	// assign a view to the player
	for (auto it = m_GameViews.begin(); it != m_GameViews.end(); ++it)
	{
		shared_ptr<IGameView> pView = (*it);
		if (pView->GetType() == GameView_Human)
		{
			shared_ptr<CityProtectorsHumanView> pHumanView = static_pointer_cast<CityProtectorsHumanView, IGameView>(pView);
			if (m_RemotePlayerId == pCastEvent->GetSocketId())
			{
				pHumanView->SetControlledGameObject(pCastEvent->GetObjectId());
			}
			return;
		}
	}

	CB_ERROR("Could not find HumanView to attach the game object to");
}

void CityProtectorsLogic::EnvironmentLoadedDelegate(IEventPtr pEvent)
{
	m_HumanGamesLoaded++;
}

void CityProtectorsLogic::StartThrustDelegate(IEventPtr pEvent)
{
	// cast the event to a start thrust event
	shared_ptr<Event_StartThrust> pCastEvent = static_pointer_cast<Event_StartThrust>(pEvent);

	// get the object that began thrusting
	StrongGameObjectPtr pObject = MakeStrongPtr(GetGameObject(pCastEvent->GetObjectId()));
	if (pObject)
	{
		// apply acceleration to the physics component of the object
		shared_ptr<PhysicsComponent> pPhysicsComponent = MakeStrongPtr(pObject->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
		if (pPhysicsComponent)
		{
			pPhysicsComponent->ApplyAcceleration(pCastEvent->GetAcceleration());
		}
	}
}

void CityProtectorsLogic::EndThrustDelegate(IEventPtr pEvent)
{
	// cast the event to an end thrust event, and get the object that stopped thrusting
	shared_ptr<Event_EndThrust> pCastEvent = static_pointer_cast<Event_EndThrust>(pEvent);
	StrongGameObjectPtr pObject = MakeStrongPtr(GetGameObject(pCastEvent->GetObjectId()));
	if (pObject)
	{
		// set the object's acceleration to 0
		shared_ptr<PhysicsComponent> pPhysicsComponent = MakeStrongPtr(pObject->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
		if (pPhysicsComponent)
		{
			pPhysicsComponent->RemoveAcceleration();
		}
	}
}

void CityProtectorsLogic::StartSteerDelegate(IEventPtr pEvent)
{
	// get the object that started steering, and apply angular acceleration to its physics component
	shared_ptr<Event_StartSteer> pCastEvent = static_pointer_cast<Event_StartSteer>(pEvent);
	StrongGameObjectPtr pObject = MakeStrongPtr(GetGameObject(pCastEvent->GetObjectId()));
	if (pObject)
	{
		shared_ptr<PhysicsComponent> pPhysicsComponent = MakeStrongPtr(pObject->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
		if (pPhysicsComponent)
		{
			pPhysicsComponent->ApplyAngularAcceleration(pCastEvent->GetAcceleration());
		}
	}
}

void CityProtectorsLogic::EndSteerDelegate(IEventPtr pEvent)
{
	// get the object that ended steering and set its angular acceleration to 0
	shared_ptr<Event_EndSteer> pCastEvent = static_pointer_cast<Event_EndSteer>(pEvent);
	StrongGameObjectPtr pObject = MakeStrongPtr(GetGameObject(pCastEvent->GetObjectId()));
	if (pObject)
	{
		shared_ptr<PhysicsComponent> pPhysicsComponent = MakeStrongPtr(pObject->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
		if (pPhysicsComponent)
		{
			pPhysicsComponent->RemoveAngularAcceleration();
		}
	}
}

bool CityProtectorsLogic::LoadGameDelegate(TiXmlElement* pLevelData)
{
	// register the game script events
	RegisterGameScriptEvents();
	return true;
}

void CityProtectorsLogic::RegisterAllDelegates()
{
	// assign delegate callbacks for events in the global event manager
	IEventManager* pGlobalEventManager = IEventManager::Get();
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::RemoteClientDelegate), Event_RemoteClient::sk_EventType);
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::MoveGameObjectDelegate), Event_MoveGameObject::sk_EventType);
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::RequestStartGameDelegate), Event_RequestStartGame::sk_EventType);
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::NetworkPlayerObjectAssignmentDelegate), Event_NetworkPlayerObjectAssignment::sk_EventType);
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::EnvironmentLoadedDelegate), Event_EnvironmentLoaded::sk_EventType);
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::EnvironmentLoadedDelegate), Event_RemoteEnvironmentLoaded::sk_EventType);

	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::StartThrustDelegate), Event_StartThrust::sk_EventType);
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::EndThrustDelegate), Event_EndThrust::sk_EventType);
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::StartSteerDelegate), Event_StartSteer::sk_EventType);
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::EndSteerDelegate), Event_EndSteer::sk_EventType);
}

void CityProtectorsLogic::RemoveAllDelegates()
{
	// remove event callbacks from the event manager
	IEventManager* pGlobalEventManager = IEventManager::Get();
	pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::RemoteClientDelegate), Event_RemoteClient::sk_EventType);
	pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::MoveGameObjectDelegate), Event_MoveGameObject::sk_EventType);
	pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::RequestStartGameDelegate), Event_RequestStartGame::sk_EventType);
	pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::NetworkPlayerObjectAssignmentDelegate), Event_NetworkPlayerObjectAssignment::sk_EventType);
	pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::EnvironmentLoadedDelegate), Event_EnvironmentLoaded::sk_EventType);
	pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::EnvironmentLoadedDelegate), Event_RemoteEnvironmentLoaded::sk_EventType);
	if (m_Proxy)
	{
		// this is only added for remote clients. its added in BaseGameLogic::SetProxy()
		pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::RequestNewGameObjectDelegate), Event_RequestNewGameObject::sk_EventType);
	}
	pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::StartThrustDelegate), Event_StartThrust::sk_EventType);
	pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::EndThrustDelegate), Event_EndThrust::sk_EventType);
	pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::StartSteerDelegate), Event_StartSteer::sk_EventType);
	pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(this, &CityProtectorsLogic::EndSteerDelegate), Event_EndSteer::sk_EventType);

}

void CityProtectorsLogic::CreateNetworkEventForwarder(const int socketId)
{
	NetworkEventForwarder* pNetworkEventForwarder = CB_NEW NetworkEventForwarder(socketId);
	IEventManager* pGlobalEventManager = IEventManager::Get();

	// add events that need to be sent along to any remote clients
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_PhysCollision::sk_EventType);
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_DestroyGameObject::sk_EventType);
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_FireWeapon::sk_EventType);
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_EnvironmentLoaded::sk_EventType);
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_NewGameObject::sk_EventType);
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_MoveGameObject::sk_EventType);
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_RequestNewGameObject::sk_EventType);
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_NetworkPlayerObjectAssignment::sk_EventType);

	// add the forwarder to the list of forwarders
	m_NetworkEventForwarders.push_back(pNetworkEventForwarder);
}

void CityProtectorsLogic::DestroyAllNetworkEventForwarders()
{
	// remove all the event listeners from the event manager, then delete all network event forwarder objects

	for (auto it = m_NetworkEventForwarders.begin(); it != m_NetworkEventForwarders.end(); ++it)
	{
		NetworkEventForwarder* pNetworkEventForwarder = (*it);
		IEventManager* pGlobalEventManager = IEventManager::Get();

		// remove these event listeners
		pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_PhysCollision::sk_EventType);
		pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_DestroyGameObject::sk_EventType);
		pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_FireWeapon::sk_EventType);
		pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_EnvironmentLoaded::sk_EventType);
		pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_NewGameObject::sk_EventType);
		pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_MoveGameObject::sk_EventType);
		pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_RequestNewGameObject::sk_EventType);
		pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_NetworkPlayerObjectAssignment::sk_EventType);

		CB_SAFE_DELETE(pNetworkEventForwarder);
	}

	m_NetworkEventForwarders.clear();
}
