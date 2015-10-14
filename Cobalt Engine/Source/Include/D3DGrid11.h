/*
	D3DGrid11.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <d3d11.h>

#include "ResourceHandle.h"
#include "SceneNode.h"
#include "Shaders.h"

/**
	A grid (for a world) rendered with Direct3D 11.
*/
class D3DGrid11 : public SceneNode
{
public:
	/// Constructor sets up the scene node
	D3DGrid11(GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent, const Mat4x4* pMatrix);
	
	/// Virtual destructor
	virtual ~D3DGrid11();

	/// Set up the grid on restore
	virtual HRESULT OnRestore(Scene* pScene);

	/// Render the grid
	virtual HRESULT Render(Scene* pScene);

	/// Update method called once per frame
	virtual HRESULT OnUpdate(Scene* pScene, float deltaTime) override { return S_OK; }

	/// Find the intersection of a ray with a polygon on the scene node
	virtual HRESULT Pick(Scene* pScene, RayCast* pRayCast) { return E_FAIL; }

	/// Return whether the texture has alpha or not
	bool HasAlpha() const { return m_TextureHasAlpha; }

public:
	/// Does the grid texture have alpha
	bool m_TextureHasAlpha;

protected:
	/// Vertex Buffer
	ID3D11Buffer* m_pVertexBuffer;
	
	/// Index Buffer
	ID3D11Buffer* m_pIndexBuffer;

	/// The compiled vertex shader
	Hlsl_VertexShader m_VertexShader;

	/// The compiled pixel shader
	Hlsl_PixelShader m_PixelShader;

	/// Number of vertices
	DWORD m_NumVerts;

	/// Number of polygons
	DWORD m_NumPolys;
};