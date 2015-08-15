/*
	GameObjectFactory.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <functional>
#include <tinyxml.h>
#include <unordered_map>

#include "interfaces.h"

/// A map that maps strings (component names stored in xml files) to functions that create components
typedef std::unordered_map<std::string, std::function<Component*()>> ComponentCreatorMap;

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
	/// Id of the last object created
	GameObjectId m_lastObjectId;
};