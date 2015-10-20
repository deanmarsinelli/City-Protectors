/*
	D3DShaderMeshNode11.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <D3D11.h>
#include <SDKmesh.h>

#include "D3DSceneNode.h"
#include "Shaders.h"

/**
	This class is used for rendering a node with a 3d mesh
	using D3D11 shaders.
*/
class D3DShaderMeshNode11 : public D3DSceneNode11
{
public:
	D3DShaderMeshNode11(const GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent,
		std::string sdkMeshFileName, RenderPass renderPass, const Mat4x4* mat);

	virtual HRESULT OnRestore(Scene* pScene);
	virtual HRESULT OnLostDevice(Scene* pScene) { return S_OK; }
	virtual HRESULT Render(Scene* pScene);
	virtual HRESULT Pick(Scene* pScene, RayCast* pRayCast);

protected:
	/// Calculate a bounding sphere for the mesh
	float CalcBoundingSphere(CDXUTSDKMesh* mesh11);

protected:
	/// Filename of the mesh used for this node
	std::string m_sdkMeshFileName;

	/// The vertex shader used for rendering
	Hlsl_VertexShader m_VertexShader;

	/// The pixel shader used for rendering
	Hlsl_PixelShader m_PixelShader;
};