/*
	GameObjectFactory.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Component.h"
#include "EngineStd.h"
#include "GameObject.h"
#include "GameObjectFactory.h"
#include "Logger.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "ScriptComponent.h"
#include "Transform.h"
#include "XmlResource.h"

GameObjectFactory::GameObjectFactory()
{
	m_lastObjectId = INVALID_GAMEOBJECT_ID;

	//TODO all components
	// Register all the component creation functions
	m_ComponentFactory.Register<MeshRenderComponent>(Component::GetIdFromName(MeshRenderComponent::g_Name));
	m_ComponentFactory.Register<PhysicsComponent>(Component::GetIdFromName(PhysicsComponent::g_Name));
	m_ComponentFactory.Register<LuaScriptComponent>(Component::GetIdFromName(LuaScriptComponent::g_Name));
	m_ComponentFactory.Register<LightRenderComponent>(Component::GetIdFromName(LightRenderComponent::g_Name));
	m_ComponentFactory.Register<SkyRenderComponent>(Component::GetIdFromName(SkyRenderComponent::g_Name));

}

StrongGameObjectPtr GameObjectFactory::CreateGameObject(const char* objectResource, TiXmlElement* overrides, const Mat4x4* pInitialTransform, const GameObjectId serversObjectId)
{
	// get the root xml node
	TiXmlElement* pRoot = XmlResourceLoader::LoadAndReturnRootXmlElement(objectResource);
	if (!pRoot)
	{
		CB_ERROR("Failed to create object from resource: " + std::string(objectResource));
		return StrongGameObjectPtr();
	}
	
	// create the game object instance
	GameObjectId nextObjectid = serversObjectId;
	if (nextObjectid == INVALID_GAMEOBJECT_ID)
	{
		nextObjectid = GetNextGameObjectId();
	}
	StrongGameObjectPtr pObject(CB_NEW GameObject(nextObjectid));
	if (!pObject->Init(pRoot))
	{
		CB_ERROR("Failed to create object: " + std::string(objectResource));
		return StrongGameObjectPtr();
	}

	bool initialTransformSet = false;

	// loop through each child element and load the components
	for (TiXmlElement* pNode = pRoot->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
	{
		// attach each component to the object
		StrongComponentPtr pComponent(CreateComponent(pNode));
		if (pComponent)
		{
			pObject->AddComponent(pComponent);
			pComponent->SetOwner(pObject);
		}
		else
		{
			// if a component cant be loaded, kill the object
			pObject->Destroy();
			return StrongGameObjectPtr();
		}
	}

	if (overrides)
	{
		ModifyGameObject(pObject, overrides);
	}

	// set the initial transform of the object
	if (pInitialTransform)
	{
		pObject->transform.SetPosition(pInitialTransform->GetPosition());
	}

	// run post-init then return the object
	pObject->PostInit();

	return pObject;
}

void GameObjectFactory::ModifyGameObject(StrongGameObjectPtr pObject, TiXmlElement* overrides)
{
	// loop through each child xml element and load the component
	for (TiXmlElement* pNode = overrides->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
	{
		ComponentId componentId = Component::GetIdFromName(pNode->Value());
		StrongComponentPtr pComponent = MakeStrongPtr(pObject->GetComponent<Component>(componentId));
	
		if (pComponent)
		{
			pComponent->Init(pNode);
			pComponent->OnChanged();
		}
		else
		{
			pComponent = CreateComponent(pNode);
			if (pComponent)
			{
				pObject->AddComponent(pComponent);
				pComponent->SetOwner(pObject);
			}
		}
	}
}

StrongComponentPtr GameObjectFactory::CreateComponent(TiXmlElement* pData)
{
	const char* name = pData->Value();

	StrongComponentPtr pComponent(m_ComponentFactory.Create(Component::GetIdFromName(name)));

	// If the component was successfully created, initialize it
	if (pComponent)
	{
		if (!pComponent->Init(pData))
		{
			CB_ERROR("Component failed to initialize: " + std::string(name));
			return StrongComponentPtr();
		}
	}
	else
	{
		CB_ERROR("Cannot find component named " + std::string(name));
		return StrongComponentPtr();
	}

	return pComponent;
}

GameObjectId GameObjectFactory::GetNextGameObjectId() 
{ 
	++m_lastObjectId;
	return m_lastObjectId;
}
