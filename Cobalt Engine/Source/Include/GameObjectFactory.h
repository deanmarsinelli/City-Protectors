/**
	GameObjectFactory.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <map>
#include <functional>
#include "tinyxml.h"
#include "interfaces.h"

/// A map that maps strings (component names stored in xml files) to functions that create components
typedef std::map<std::string, std::function<Component*>()> ComponentCreatorMap;

/**
	TODO: documentation
*/
class GameObjectFactory
{
public:
	GameObjectFactory();
	StrongGameObjectPtr CreateGameObject(const char* objectResource);

protected:
	virtual StrongComponentPtr CreateComponent(TiXmlElement* pData);

private:
	GameObjectId GetNextGameObjectId();

protected:
	ComponentCreatorMap m_componentFactory;

private:
	GameObjectId m_lastObjectId;
};