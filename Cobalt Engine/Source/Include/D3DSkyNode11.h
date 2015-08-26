/*
	D3DSkyNode11.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <D3D11.h>

#include "Shaders.h"
#include "SkyNode.h"

/**
	Sky box implementation for a D3D11 renderer.
*/
class D3DSkyNode11 : public SkyNode
{
public:
	/// Constructor taking a texture name
	D3DSkyNode11(const char* pTextureBaseName);
	
	/// Virtual destructor
	virtual ~D3DSkyNode11();

	/// Called when device is restored
	HRESULT OnRestore(Scene* pScene);

	/// Render the node
	HRESULT Render(Scene* pScene);

protected:
	/// The index buffer
	ID3D11Buffer*	m_pIndexBuffer;

	// The vertex buffer
	ID3D11Buffer*	m_pVertexBuffer;

	/// The vertex shader
	Hlsl_VertexShader m_VertexShader;

	/// The pixel shader
	Hlsl_PixelShader m_PixelShader;
};
