/**
	GameObject.cpp

	GameObjects represent anything that can
	exist in the engine. These are also commonly
	referred to as Actors in other engines.

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "GameObject.h"
#include "Component.h"

GameObject::GameObject(GameObjectId id)
{
	m_Id = id;
	m_Type = "Unknown";
	m_Resource = "Unknown";
}

GameObject::~GameObject()
{
	// if this assert fires, then Destroy() was not called 
	assert(m_Components.empty());
}

bool GameObject::Init(TiXmlElement* pData)
{
	m_Type = pData->Attribute("type");
	m_Resource = pData->Attribute("resource");

	return true;
}

void GameObject::PostInit()
{
	// initialize all components
	for (Components::iterator it = m_Components.begin(); it != m_Components.end(); ++it)
	{
		it->second->PostInit();
	}
}

void GameObject::AddComponent(StrongComponentPtr pComponent)
{
	m_Components.insert(std::make_pair(pComponent->GetId(), pComponent));
}