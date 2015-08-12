/*
	BaseGameLogic.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "interfaces.h"
#include "GameObject.h"

typedef std::map<GameObjectId, StrongGameObjectPtr> GameObjectMap;

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


class BaseGameLogic
{
public:	
	virtual weak_ptr<GameObject> GetGameObject(const GameObjectId id);
	GameViewList& GetGameViewList();
	
protected:
	GameObjectMap m_Objects;
	GameViewList m_GameViews;
};
