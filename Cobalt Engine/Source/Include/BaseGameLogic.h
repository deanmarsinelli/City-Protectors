/*
	BaseGameLogic.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "GameObject.h"

typedef std::map<GameObjectId, StrongGameObjectPtr> GameObjectMap;

class BaseGameLogic
{
public:
	virtual weak_ptr<GameObject> GetObject(const GameObjectId id);

private:
	GameObjectMap m_Objects;
};