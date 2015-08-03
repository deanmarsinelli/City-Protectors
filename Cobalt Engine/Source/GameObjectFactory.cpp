/*
	GameObjectFactory.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "GameObjectFactory.h"
#include "Component.h"

GameObjectFactory::GameObjectFactory()
{
	// Register all the create functions
}

StrongGameObjectPtr GameObjectFactory::CreateGameObject(const char* objectResource)
{
	// TODO: finish this p162
}

StrongComponentPtr GameObjectFactory::CreateComponent(TiXmlElement* pData)
{
	const char* name = pData->Value();

	StrongComponentPtr pComponent = nullptr;

	// Search the map for a function that can create this component
	auto it = m_componentFactory.find(name);
	if (it != m_componentFactory.end())
	{
		auto componentCreator = it->second;
		Component* com = componentCreator();
		pComponent.reset(com);
	}

	// If the component was successfully created, initialize it
	if (pComponent)
	{
		if (!pComponent->Init(pData))
		{
			// TODO: error failed to init
		}
	}
	else
	{
		// TODO: error cant find component
	}

	return pComponent;
}

GameObjectId GameObjectFactory::GetNextGameObjectId() 
{ 
	++m_lastObjectId;
	return m_lastObjectId;
}