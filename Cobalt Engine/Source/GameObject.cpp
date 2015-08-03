/*
	GameObject.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <string>
#include "GameObject.h"
#include "Component.h"
#include "Logger.h"
#include "StringUtil.h"

GameObject::GameObject(GameObjectId id)
{
	m_Id = id;
	m_Type = "Unknown";
	m_Resource = "Unknown";
}

GameObject::~GameObject()
{
	CB_LOG("Object", std::string("Destroying Object ") + ToStr(m_Id));
	// if this assert fires, then Destroy() was not called on the Object before deleting it (BAD)
	CB_ASSERT(m_Components.empty());
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