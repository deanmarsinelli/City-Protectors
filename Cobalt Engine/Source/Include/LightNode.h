/*
	LightNode.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "SceneNode.h"

/// Properties of all light nodes
struct LightProperties
{
	float m_Attenuation[3];
	float m_Range;
	float m_Falloff;
	float m_Theta;
	float m_Phi;
};


/**
	This is the base class for a light node that exists in a scene graph.
*/
class LightNode : public SceneNode
{
public:
	/// Constructor to build a light node
	LightNode(const GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent, const LightProperties& properties, const Mat4x4* mat);

protected:
	/// Properties for the light node
	LightProperties m_LightProperties;
};


/**
	Light node implementation for a D3D9 renderer.
*/
class D3DLightNode9 : public LightNode
{
public:
	/// Constructor builds a direct3d9 light
	D3DLightNode9(const GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent, const LightProperties& properties, const Mat4x4* mat) :
		LightNode(objectId, renderComponent, properties, mat)
	{}

	/// Called when restoring the device
	virtual HRESULT OnRestore(Scene* pScene);

	/// Update method called once per frame
	virtual HRESULT OnUpdate(Scene* pScene, float deltaTime);

	/// The d3d9 light
	D3DLIGHT9 m_d3dLight9;
};


/**
	Light node implementation for a D3D11 renderer.
*/
class D3DLightNode11 : public LightNode
{
public:
	/// Constructor builds a direct3d11 light
	D3DLightNode11(const GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent, const LightProperties& properties, const Mat4x4* mat) :
		LightNode(objectId, renderComponent, properties, mat)
	{}

	/// Called when restoring the device
	virtual HRESULT OnRestore() { return S_OK; }

	/// Update method called once per frame
	virtual HRESULT OnUpdate(Scene* pScene, float deltaTime);
};
