/*
	GameObject.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <tinyxml.h>
#include <unordered_map>

#include "interfaces.h"
#include "Transform.h"

/**
	Represents any object that can exist in the game world. This class is not meant
	to be subclassed, instead use components to compose your game objects.
*/
class GameObject
{
	friend class GameObjectFactory;
	typedef std::unordered_map<ComponentId, StrongComponentPtr> Components;
	typedef std::string GameObjectType;

public:
	explicit GameObject(GameObjectId id);
	~GameObject();

	bool Init(TiXmlElement* pData);
	void PostInit();
	void Destroy();
	void Update(const float deltaTime);

	/**
		Return the id of the game object
	*/
	GameObjectId GetId() const 
	{ 
		return m_Id; 
	}

	/**
		Return the type of the game object
	*/
	GameObjectType GetType() const 
	{ 
		return m_Type; 
	}

	/**
		Return a read only pointer to the map of components
	*/
	const Components* GetComponents()
	{
		return &m_Components;
	}

	/**
		Return a weak pointer to a particular component by id
	*/
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
	
	/**
		Return a weak pointer to a particular component by name
	*/
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

private:
	/**
		Add a component to the Game Object. This should only be called by the Game Object Factory
	*/
	void AddComponent(StrongComponentPtr pComponent);

public:
	Transform transform;

private:
	/// Unique id for the game object
	GameObjectId m_Id;

	/// Type of game object stored as a string
	GameObjectType m_Type;

	/// Map of component id's to components
	Components m_Components;

	/// The xml file from which the object was instantiated
	std::string m_Resource;
};