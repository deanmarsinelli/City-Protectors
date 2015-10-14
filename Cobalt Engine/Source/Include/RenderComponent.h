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
	Represents a render component built from a mesh.
*/
class MeshRenderComponent : public BaseRenderComponent
{
public:
	virtual const char* GetName() const;

protected:
	/// Factory method to create appropriate scene node
	virtual shared_ptr<SceneNode> CreateSceneNode() override;

	/// Editor method
	virtual void CreateInheritedXmlElements(TiXmlElement* pBaseElement);

public:
	/// Name of the component
	static const char* g_Name;
};


/**
	Render component for lights.
*/
class LightRenderComponent : public BaseRenderComponent
{
public:
	/// Default Constructor
	LightRenderComponent() { }

	/// Return the name of the component
	virtual const char* GetName() const;

protected:
	virtual bool DelegateInit(TiXmlElement* pData) override;

	/// Factory method to create appropriate scene node
	virtual shared_ptr<SceneNode> CreateSceneNode() override;

	/// Editor method
	virtual void CreateInheritedXmlElements(TiXmlElement* pBaseElement);

public:
	/// Name of the component
	static const char* g_Name;
	
private:
	/// Properties of a light component
	LightProperties m_Properties;
};


/**
	Render component for sky objects.
*/
class SkyRenderComponent : public BaseRenderComponent
{
public:
	/// Default constructor
	SkyRenderComponent() { }

	/// Return the name of the component
	virtual const char* GetName() const;

protected:
	virtual bool DelegateInit(TiXmlElement* pData) override;

	/// Factory method to create appropriate scene node
	virtual shared_ptr<SceneNode> CreateSceneNode() override;

	/// Editor method
	virtual void CreateInheritedXmlElements(TiXmlElement* pBaseElement);

public:
	// Name of the component
	static const char* g_Name;

private:
	/// Name of the texture resource for the sky
	std::string m_TextureResource;
};


//====================================================
//	Some built in default render components
//====================================================

/**
	A grid representing the world.
*/
class GridRenderComponent : public BaseRenderComponent
{
public:
	/// Default constructor
	GridRenderComponent();

	/// Return the name of the component
	virtual const char *GetName() const { return g_Name; }
	
	/// Return the name of the texture resource
	const char* GetTextureResource() { return m_TextureResource.c_str(); }

	const int GetDivision() { return m_Squares; }

protected:
	virtual bool DelegateInit(TiXmlElement* pData) override;

	/// Factory method to create appropriate scene node
	virtual shared_ptr<SceneNode> CreateSceneNode() override;

	/// Editor method
	virtual void CreateInheritedXmlElements(TiXmlElement* pBaseElement);

public:
	/// Name of the component
	static const char* g_Name;

private:
	/// Name of the texture resource for the grid
	std::string m_TextureResource;

	int m_Squares;
};