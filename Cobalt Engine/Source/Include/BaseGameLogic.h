/*
	BaseGameLogic.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <unordered_map>

#include "interfaces.h"
#include "Process.h"

typedef std::unordered_map<GameObjectId, StrongGameObjectPtr> GameObjectMap;

enum BaseGameState
{
	Invalid,
	Initializing,
	MainMenu,
	WaitingForPlayers,
	LoadingGameEnvironment,
	WaitingForPlayersToLoadEnvironment,
	SpawningPlayersObjects,
	Running
};

class IGamePhysics;

class BaseGameLogic
{
public:	
	virtual weak_ptr<GameObject> GetGameObject(const GameObjectId id);
	GameViewList& GetGameViewList();
	
	void OnUpdate(float x,float y) { }
	void RenderDiagnostics() {}

	virtual StrongGameObjectPtr CreateGameObject(const std::string &actorResource, TiXmlElement *overrides, const Mat4x4* initialTransform = NULL, const GameObjectId serversActorId = INVALID_GAMEOBJECT_ID){ }
	void AttachProcess(StrongProcessPtr pProcess) { }
	virtual shared_ptr<IGamePhysics> GetGamePhysics(void) { }

protected:
	GameObjectMap m_Objects;
	GameViewList m_GameViews;
};
