/*
	D3DSceneNode.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "SceneNode.h"

/**
	A scene node implementation for a D3D9 renderer.
*/
class D3DSceneNode9 : public SceneNode
{
public:
	D3DSceneNode9(const GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent,
		RenderPass renderPass, const Mat4x4* mat) :
		SceneNode(objectId, renderComponent, renderPass, mat)
	{}

	virtual HRESULT Render(Scene* pScene);
};


/**
	A scene node implementation for a D3D11 renderer.
*/
class D3DSceneNode11 : public SceneNode
{
public:
	virtual HRESULT Render(Scene* pScene) { return S_OK; }
};