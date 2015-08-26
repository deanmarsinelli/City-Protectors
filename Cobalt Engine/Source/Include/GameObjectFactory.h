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
#include "Matrix.h"
#include "templates.h"

/**
	This class is used to create game objects by using xml data to 
	attach components to them.
*/
class GameObjectFactory
{
public:
	/// Default constructor
	GameObjectFactory();

	/// Create a game object from a resource
	StrongGameObjectPtr CreateGameObject(const char* objectResource, TiXmlElement* overrides, const Mat4x4* pInitialTransform, const GameObjectId serversObjectId);

	/// Modify a game object's components
	void ModifyGameObject(StrongGameObjectPtr pObject, TiXmlElement* overrides);

protected:
	/// Create a component from xml data
	virtual StrongComponentPtr CreateComponent(TiXmlElement* pData);

private:
	/// Returns the next object id
	GameObjectId GetNextGameObjectId();

protected:
	/// Factory to create components
	GenericObjectFactory<Component, ComponentId> m_ComponentFactory;

private:
	/// Id of the last object created
	GameObjectId m_lastObjectId;
};
