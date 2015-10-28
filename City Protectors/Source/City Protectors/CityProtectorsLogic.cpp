/*
	CityProtectorsLogic.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <GameObject.h>
#include <Events.h>
#include <Logger.h>
#include <NetworkGameView.h>
#include <Physics.h>
#include <Transform.h>

#include "AIView.h"
#include "CityProtectorsHumanView.h"
#include "CityProtectorsLogic.h"

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
		Transform* transform = &(pObject->transform);

		if (transform->GetPosition().y < -25)
		{
			shared_ptr<Event_DestroyGameObject> pDestroyActorEvent(CB_NEW Event_DestroyGameObject(id));
			IEventManager::Get()->QueueEvent(pDestroyActorEvent);
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

	case BaseGameState::SpawningPlayersObjects:
		if (m_Proxy)
		{
			// only the server needs to spawn player objects
			return;
		}

		for (auto it = m_GameViews.begin(); it != m_GameViews.end(); ++it)
		{
			shared_ptr<IGameView> pView = (*it);
			if (pView->GetType == GameView_Human)
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

// todo delegates
