/*
	BaseGameLogic.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <FastDelegate.h>
#include <tinyxml.h>

#include "BaseGameLogic.h"
#include "EngineStd.h"
#include "Events.h"
#include "GameObjectFactory.h"
#include "LevelManager.h"
#include "Logger.h"
#include "ProcessManager.h"
#include "ResourceCache.h"
#include "StringUtil.h"
#include "templates.h"

BaseGameLogic::BaseGameLogic()
{
	m_LastObjectId = 0;
	m_LifeTime = 0.0f;
	m_pProcessManager = CB_NEW ProcessManager;
	m_Random.Randomize();
	m_State = BaseGameState::Initializing;
	m_Proxy = false;
	m_RenderDiagnostics = false;
	m_ExpectedPlayers = 0;
	m_ExpectedRemotePlayers = 0;
	m_ExpectedAI = 0;
	m_HumanPlayersAttached = 0;
	m_AIPlayersAttached = 0;
	m_HumanGamesLoaded = 0;
	m_pPathingGraph = nullptr;
	m_pObjectFactory = nullptr;

	m_pLevelManager = CB_NEW LevelManager;

	CB_ASSERT(m_pProcessManager && m_pLevelManager);
	m_pLevelManager->Initialize(g_pApp->m_ResCache->Match("world\\*.xml"));
	
	// register script events from the engine
	RegisterEngineScriptEvents();
}

BaseGameLogic::~BaseGameLogic()
{
	// remove views from the game logic list
	while (!m_GameViews.empty())
	{
		m_GameViews.pop_front();
	}
	
	CB_SAFE_DELETE(m_pLevelManager);
	CB_SAFE_DELETE(m_pProcessManager);
	CB_SAFE_DELETE(m_pObjectFactory);

	// destroy all game objects
	for (auto it = m_Objects.begin(); it != m_Objects.end(); ++it)
	{
		it->second->Destroy();
	}
	m_Objects.clear();

	IEventManager::Get()->RemoveListener(fastdelegate::MakeDelegate(this, &BaseGameLogic::RequestDestroyGameObjectDelegate), Event_RequestDestroyGameObject::sk_EventType);
}

bool BaseGameLogic::Init()
{
	m_pObjectFactory = CreateObjectFactory();
	m_pPathingGraph.reset(CreatePathingGraph());

	IEventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &BaseGameLogic::RequestDestroyGameObjectDelegate), Event_RequestDestroyGameObject::sk_EventType);

	return true;
}

void BaseGameLogic::SetProxy(bool isProxy)
{
	m_Proxy = isProxy;
}

const bool BaseGameLogic::IsProxy() const
{
	return m_Proxy;
}

const bool BaseGameLogic::CanRunLua() const
{
	return !IsProxy() || GetState() != BaseGameState::Running;
}

GameViewList& BaseGameLogic::GetGameViewList()
{
	return m_GameViews;
}

GameObjectId BaseGameLogic::GetNewObjectId()
{
	return ++m_LastObjectId;
}

shared_ptr<PathingGraph> BaseGameLogic::GetPathingGraph()
{
	return m_pPathingGraph;
}

CBRandom& BaseGameLogic::GetRNG()
{
	return m_Random;
}

void BaseGameLogic::AddView(shared_ptr<IGameView> pView, GameObjectId id)
{
	// add the view to the game view list and initialize it
	int viewId = static_cast<int>(m_GameViews.size());
	m_GameViews.push_back(pView);
	pView->OnAttach(viewId, id);
	pView->OnRestore();
}

void BaseGameLogic::RemoveView(shared_ptr<IGameView> pView)
{
	m_GameViews.remove(pView);
}

WeakGameObjectPtr BaseGameLogic::GetGameObject(const GameObjectId id)
{
	// find the game object and return it
	GameObjectMap::iterator findIt = m_Objects.find(id);
	if (findIt != m_Objects.end())
	{
		return findIt->second;
	}

	return WeakGameObjectPtr();
}

StrongGameObjectPtr BaseGameLogic::CreateGameObject(const std::string& objectResource, TiXmlElement* overrides,
const Mat4x4* initialTransform = nullptr, const GameObjectId serversObjectId)
{
	CB_ASSERT(m_pObjectFactory);

	if (!m_Proxy && serversObjectId != INVALID_GAMEOBJECT_ID)
		return StrongGameObjectPtr();

	if (m_Proxy && serversObjectId == INVALID_GAMEOBJECT_ID)
		return StrongGameObjectPtr();

	StrongGameObjectPtr pObject = m_pObjectFactory->CreateGameObject(objectResource.c_str(), overrides, initialTransform, serversObjectId);
	if (pObject)
	{
		m_Objects.insert(std::make_pair(pObject->GetId(), pObject));
		if (!m_Proxy && (m_State == BaseGameState::SpawningPlayersObjects || m_State == BaseGameState::Running))
		{
			// broadcast an event that a new object was created
			shared_ptr<Event_RequestNewGameObject> pNewObject(CB_NEW Event_RequestNewGameObject(objectResource, initialTransform, pObject->GetId()));
			IEventManager::Get()->TriggerEvent(pNewObject);
		}
		return pObject;
	}
	else
	{
		// couldnt create object
		return StrongGameObjectPtr();
	}
}

void BaseGameLogic::DestroyGameObject(const GameObjectId id)
{
	// trigger an event so that any systems resonding to the event can still
	// access a valid game object, then destroy it completely
	shared_ptr<Event_DestroyGameObject> pEvent(CB_NEW Event_DestroyGameObject(id));
	IEventManager::Get()->TriggerEvent(pEvent);

	auto findIt = m_Objects.find(id);
	if (findIt != m_Objects.end())
	{
		findIt->second->Destroy();
		m_Objects.erase(findIt);
	}
}

void BaseGameLogic::ModifyGameObject(const GameObjectId id, TiXmlElement* overrides)
{
	CB_ASSERT(m_pObjectFactory);
	if (!m_pObjectFactory)
		return;

	auto findIt = m_Objects.find(id);
	if (findIt != m_Objects.end())
	{
		m_pObjectFactory->ModifyObject(findIt->second, overrides);
	}
}

std::string BaseGameLogic::GetGameObjectXml(const GameObjectId id)
{
	StrongGameObjectPtr pObject = MakeStrongPtr(GetGameObject(id));
	if (pObject)
		return pObject->ToXML();
	else
		CB_ERROR("Couldn't find object: " + ToStr(id));

	return std::string();
}

bool BaseGameLogic::LoadGame(const char* levelResource)
{
	// TODO
}

const LevelManager* BaseGameLogic::GetLevelManager()
{
	return m_pLevelManager;
}

void BaseGameLogic::SetProxy()
{
	m_Proxy = true;
	IEventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &BaseGameLogic::RequestNewGameObjectDelegate), Event_RequestNewGameObject::sk_EventType);

	m_pPhysics.reset(CreateNullPhysics());
}

void BaseGameLogic::OnUpdate(float time, float deltaTime)
{
	m_LifeTime += deltaTime;

	switch (m_State)
	{
	case BaseGameState::Initializing:
		// done initializing
		ChangeState(BaseGameState::MainMenu);
		break;

	case BaseGameState::MainMenu:
		break;

	case BaseGameState::LoadingGameEnvironment:
		break;

	case BaseGameState::WaitingForPlayersToLoadEnvironment:
		if (m_ExpectedPlayers + m_ExpectedRemotePlayers <= m_HumanGamesLoaded)
		{
			ChangeState(BaseGameState::SpawningPlayersObjects);
		}
		break;

	case BaseGameState::SpawningPlayersObjects:
		ChangeState(BaseGameState::Running);
		break;

	case BaseGameState::WaitingForPlayers:
		if (m_ExpectedPlayers + m_ExpectedRemotePlayers == m_HumanPlayersAttached)
		{
			if (!g_pApp->m_Options.m_Level.empty())
			{
				ChangeState(BaseGameState::LoadingGameEnvironment);
			}
		}
		break;

	case BaseGameState::Running:
		m_pProcessManager->UpdateProcesses(deltaTime);
		if (m_pPhysics && !m_Proxy)
		{
			m_pPhysics->OnUpdate(deltaTime);
			m_pPhysics->SyncVisibleScene();
		}
		break;

	default:
		CB_ERROR("Unrecognized state");
	}

	// update all game views
	for (auto it = m_GameViews.begin(); it != m_GameViews.end(); ++it)
	{
		(*it)->OnUpdate(deltaTime);
	}

	// update all game objects
	for (auto it = m_Objects.begin(); it != m_Objects.end(); ++it)
	{
		it->second->Update(deltaTime);
	}
}

void BaseGameLogic::ChangeState(enum BaseGameState newState)
{
	// if changing to waiting for players
	if (newState == BaseGameState::WaitingForPlayers)
	{
		// get rid of the main menu
		m_GameViews.pop_front();

		m_ExpectedPlayers = 1;
		m_ExpectedRemotePlayers = g_pApp->m_Options.m_ExpectedPlayers - 1;
		m_ExpectedAI = g_pApp->m_Options.m_NumAIs;

		if (!g_pApp->m_Options.m_GameHost.empty())
		{
			SetProxy();
			m_ExpectedAI = 0;
			m_ExpectedRemotePlayers = 0;
			if (!g_pApp->AttachAsClient())
			{
				// throw up main menu if couldnt connect
				ChangeState(BaseGameState::MainMenu);
				return;
			}
		}
		else if (m_ExpectedRemotePlayers > 0)
		{
			BaseSocketManager* pServer = CB_NEW BaseSocketManager();
			if (!pServer->Init())
			{
				// throw up main menu if could not init server
				ChangeState(BaseGameState::MainMenu);
				return;
			}
			pServer->AddSocket(new GameServerListenSocket(g_pApp->m_Options.m_ListenPort));
			g_pApp->m_pBaseSocketManger = pServer;
		}
	}
	else if (newState == BaseGameState::LoadingGameEnvironment)
	{
		m_State = newState;
		if (!g_pApp->LoadGame())
		{
			CB_ERROR("Game failed to load.");
			g_pApp->AbortGame();
		}
		else
		{
			ChangeState(BaseGameState::WaitingForPlayersToLoadEnvironment);
			return;
		}
	}

	m_State = newState;
}

const BaseGameState BaseGameLogic::GetState() const
{
	return m_State;
}

void BaseGameLogic::ToggleRenderDiagnostics()
{
	m_RenderDiagnostics = !m_RenderDiagnostics;
}

void BaseGameLogic::RenderDiagnostics()
{
	if (m_RenderDiagnostics)
	{
		m_pPhysics->RenderDiagnostics();
	}
}

shared_ptr<IGamePhysics> BaseGameLogic::GetGamePhysics()
{
	return m_pPhysics;
}

void BaseGameLogic::AttachProcess(StrongProcessPtr pProcess)
{
	m_pProcessManager->AttachProcess(pProcess);
}

void BaseGameLogic::RequestDestroyGameObjectDelegate(IEventPtr pEvent)
{
	shared_ptr<Event_RequestDestroyGameObject> pCastEvent = static_pointer_cast<Event_RequestDestroyGameObject>(pEvent);
	DestroyGameObject(pCastEvent->GetId());
}

GameObjectFactory* BaseGameLogic::CreateObjectFactory()
{
	return CB_NEW GameObjectFactory;
}

bool BaseGameLogic::LoadGameDelegate(TiXmlElement* pLevelDate)
{
	return true;
}

void BaseGameLogic::MoveGameObjectDelegate(IEventPtr pEvent)
{
	shared_ptr<Event_MoveGameObject> pCastEvent = static_pointer_cast<Event_MoveGameObject>(pEvent);
	MoveGameObject(pCastEvent->GetId(), pCastEvent->GetMatrix());
}

void BaseGameLogic::RequestNewGameObjectDelegate(IEventPtr pEvent)
{
	CB_ASSERT(m_Proxy);
	if (!m_Proxy)
		return;

	// pull the object out of the event
	shared_ptr<Event_RequestNewGameObject> pCastEvent = static_pointer_cast<Event_RequestNewGameObject>(pEvent);
	StrongGameObjectPtr pObject = CreateGameObject(pCastEvent->GetObjectResource(), nullptr, pCastEvent->GetInitialTransform(), pCastEvent->GetServerObjectId());
	if (pObject)
	{
		// add the object to the logic and broadcast this message
		shared_ptr<Event_NewGameObject> pNewObjectEvent(CB_NEW Event_NewGameObject(pObject->GetId(), pCastEvent->GetViewId()));
		IEventManager::Get()->QueueEvent(pNewObjectEvent);
	}
}
