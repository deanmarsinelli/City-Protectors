/**
	GameObject.h

	GameObjects represent anything that can
	exist in the engine. These are also commonly
	referred to as Actors in other engines.

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <map>
#include "EngineStd.h"
#include "interfaces.h"

/**
	Reprensents any object that can exist in the game world.
*/
class GameObject
{
private:
	friend class GameObjectFactory;
	typedef std::map<ComponentId, StrongComponentPtr> Components;
	typedef std::string GameObjectType;

public:
	explicit GameObject(GameObjectId id);
	~GameObject();

	bool Init(TiXmlElement* pData);
	void PostInit();
	void Destroy();
	void Update(const float deltaTime);

	void AddComponent(StrongComponentPtr pComponent);

	// template functions for getting a component
	template <typename ComponentType>
	weak_ptr<ComponentType> GetComponent(ComponentId id)
	{
		// see if the component id exists
		Components::iterator it = m_Components.find(id);
		if (it != m_Components.end())
		{
			// if it exists, get the component part
			StrongComponentPtr pBase(it->second);
			// cast it down to the derived class and return a weak ptr
			shared_ptr<ComponentType> pDerived(static_pointer_cast<ComponentType>(pBase));
			weak_ptr<ComponentType> pWeakDerived(pDerived);
			return pWeakDerived;
		}
		else
		{
			return nullptr;
		}
	}

	template <typename ComponentType>
	weak_ptr<ComponentType> GetComponent(const char* name)
	{
		ComponentId id = Component::GetIdFromName(name);
		Components::iterator it = m_Components.find(id);
		if (it != m_Components.end())
		{
			// if it exists, get the component part
			StrongComponentPtr pBase(it->second);
			// cast it down to the derived class and return a weak ptr
			shared_ptr<ComponentType> pDerived(static_pointer_cast<ComponentType>(pBase));
			weak_ptr<ComponentType> pWeakDerived(pDerived);
			return pWeakDerived;
		}
		else
		{
			return nullptr;
		}
	}

	const Components* GetComponents();

private:
	GameObjectId m_Id;
	GameObjectType m_Type;
	Components m_Components;

	std::string m_Resource;
};