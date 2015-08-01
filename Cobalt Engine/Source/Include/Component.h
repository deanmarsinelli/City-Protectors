/**
	Component.h

	Components represent attachments to game
	objects and must be derived from the
	Component base class.

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "EngineStd.h"
#include "interfaces.h"
#include "StringUtil.h"

/**
	Represents components that can be attached to Game Objects to run custom logic.
*/
class Component
{
	friend class GameObjectFactory;
public:
	virtual ~Component();

	virtual bool Init(TiXmlElement* pData) = 0;
	virtual void PostInit() { }
	virtual void Update(const float deltaTime) { }
	virtual void OnChange() { }

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
	StrongGameObjectPtr m_pOwner;
};