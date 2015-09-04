/*
	Component.h

	Components represent attachments to game
	objects and must be derived from the
	Component base class.

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <tinyxml.h>

#include "interfaces.h"
#include "StringUtil.h"

/**
	Represents components that can be attached to Game Objects to run custom logic.
	Each component has a unique identifier and a game object can only have one component
	of a particular type.
*/
class Component
{
	friend class GameObjectFactory;

public:
	/// Default destructor releases the parent pointer
	virtual ~Component() 
	{
		m_pOwner.reset();
	}

	/// Initialize a component from xml data
	virtual bool Init(TiXmlElement* pData) = 0;

	/// Handle any logic after initialization
	virtual void PostInit() {}

	/// Update method called once per frame
	virtual void Update(float deltaTime) {}

	/// Update a component when it is changed (the object is modified)
	virtual void OnChanged() {}

	/// Generate xml from the component
	virtual TiXmlElement* GenerateXml() = 0;

	/// Return the id of the component
	virtual ComponentId GetId() const
	{
		return GetIdFromName(GetName());
	}

	/// Return the name of the component
	virtual const char* GetName() const = 0;

	/// Return a component id given the name
	static ComponentId GetIdFromName(const char* name)
	{
		void* rawId = HashedString::Hash_Name(name);
		return reinterpret_cast<ComponentId>(rawId);
	}

private:
	/// Set the owner game object of the component
	void SetOwner(StrongGameObjectPtr pOwner)
	{
		m_pOwner = pOwner;
	}

protected:
	/// Strong reference to the object this component is attached to
	StrongGameObjectPtr m_pOwner;
};
