/*
	RenderComponent.h

	This file contains many types of rendering related
	components that the engine supports.

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "Component.h"
#include "interfaces.h"
#include "LightNode.h"
#include "SceneNode.h"

/**
	Rendering component interface. All rendering components derive from this class.
*/
class RenderComponentInterface :public Component
{
	virtual shared_ptr<SceneNode> GetSceneNode() = 0;
};


/**
	Base class implementing the render component interface. The rest of the render
	components derive from the base render component class.
*/
class BaseRenderComponent : public RenderComponentInterface
{
public:
	virtual bool Init(TiXmlElement* pData) override;
	virtual void PostInit() override;
	virtual void OnChanged() override;
	virtual TiXmlElement* GenerateXml() override;
	const Color GetColor() const;
	
protected:
	virtual bool DelegateInit(TiXmlElement* pData);

	/// Factory method to create appropriate scene node
	virtual shared_ptr<SceneNode> CreateSceneNode() = 0;
	Color LoadColor(TiXmlElement* pData);

	// editor stuff
	virtual TiXmlElement* CreateBaseElement();
	virtual void CreateInheritedXmlElements(TiXmlElement* pBaseElement) = 0;

private:
	virtual shared_ptr<SceneNode> GetSceneNode() override;

protected:
	shared_ptr<SceneNode> m_pSceneNode;
	Color m_Color;
};

/**
	Render component for lights.
*/
class LightRenderComponent : public BaseRenderComponent
{
public:
	LightRenderComponent();
	virtual const char* GetName() const;

protected:
	virtual bool DelegateInit(TiXmlElement* pData) override;
	virtual shared_ptr<SceneNode> CreateSceneNode() override;

	virtual void CreateInheritedXmlElements(TiXmlElement* pBaseElement);
public:
	static const char* g_Name;
	
private:
	LightProperties m_Properties;
};