/*
	D3DSkyNode9.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "SkyNode.h"

/**
	Sky box implementation for a D3D9 renderer.
*/
class D3DSkyNode9 : public SkyNode
{
public:
	/// Constructor taking in a texture name
	D3DSkyNode9(const char* pTextureBaseName);

	/// Virtual destructor
	virtual ~D3DSkyNode9();

	/// Called when device is restored
	HRESULT OnRestore(Scene* pScene);

	/// Render the node
	HRESULT Render(Scene* pScene);

protected:
	/// The sky textures
	LPDIRECT3DTEXTURE9 m_pTexture[5];

	/// The sky vertices
	LPDIRECT3DVERTEXBUFFER9 m_pVerts;
};