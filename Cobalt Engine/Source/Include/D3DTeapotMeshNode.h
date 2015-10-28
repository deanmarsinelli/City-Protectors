/*
	D3DTeapotMeshNode.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "D3DShaderMeshNode9.h"
#include "D3DShaderMeshNode11.h"

/**
	A D3D9 scene graph object representing a teapot.
*/
class D3DTeapotMeshNode9 : public D3DShaderMeshNode9
{
public:
	/// Constructor pairing the mesh node with a game object
	D3DTeapotMeshNode9(const GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent, std::string fxFileName, RenderPass renderPass, const Mat4x4 *t) :
		D3DShaderMeshNode9(objectId, renderComponent, NULL, fxFileName, renderPass, t) { }

	/// Initialize/Restore the node
	virtual HRESULT OnRestore(Scene* pScene);
};


/**
	A D3D11 scene graph object representing a teapot.
*/
class D3DTeapotMeshNode11 : public D3DShaderMeshNode11
{
public:
	/// Constructor pairing the sdkmesh with a game object
	D3DTeapotMeshNode11(const GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent, RenderPass renderPass, const Mat4x4 *t) : 
		D3DShaderMeshNode11(objectId, renderComponent, "art\\teapot.sdkmesh", renderPass, t) { }

	/// Return true the node is visible
	bool IsVisible() { return true; }
};
