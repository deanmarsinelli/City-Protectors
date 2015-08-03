/*
	Component.h

	Components represent attachments to game
	objects and must be derived from the
	Component base class.

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

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
	virtual ~Component() 
	{
		m_pOwner.reset();
	}

	/// These methods are meant to be overridden
	virtual bool Init(TiXmlElement* pData) = 0;
	virtual void PostInit() {}
	virtual void Update(const float deltaTime) {}
	virtual void OnChange() {}

	virtual ComponentId GetId() const
	{
		return GetIdFromName(GetName());
	}
	virtual const char* GetName() const = 0;
	static ComponentId GetIdFromName(const char* name)
	{
		void* rawId = HashedString::hash_name(name);
		return reinterpret_cast<ComponentId>(rawId);
	}

private:
	void SetOwner(StrongGameObjectPtr pOwner)
	{
		m_pOwner = pOwner;
	}

protected:
	/// Strong reference to the object this component is attached to
	StrongGameObjectPtr m_pOwner;
};