/*
	GameObject.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <string>

#include "Component.h"
#include "GameObject.h"
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
	CB_LOG("Object", std::string("Initializing Object: ") + ToStr(m_Id));
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

void GameObject::Destroy()
{
	// clear all the components
	m_Components.clear();
}

void GameObject::Update(float deltaTime)
{
	// call update on all components
	for (auto it = m_Components.begin(); it != m_Components.end(); ++it)
	{
		it->second->Update(deltaTime);
	}
}

GameObjectId GameObject::GetId() const
{
	return m_Id;
}

GameObject::GameObjectType GameObject::GetType() const
{
	return m_Type;
}

const GameObject::Components* GameObject::GetComponents()
{
	return &m_Components;
}

void GameObject::AddComponent(StrongComponentPtr pComponent)
{
	m_Components.insert(std::make_pair(pComponent->GetId(), pComponent));
}