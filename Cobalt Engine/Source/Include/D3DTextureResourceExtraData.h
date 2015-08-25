/*
	D3DTextureResourceExtraData.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <d3d9.h>
#include <D3D11.h>
#include <string>

#include "interfaces.h"

/**
	Extra data that is attached to a d3d9 texture resource. This includes
	the actual D3DTexture9 object.
*/
class D3DTextureResourceExtraData9 : public IResourceExtraData
{
	friend class D3DTextureResourceLoader;
	
public:
	/// Default constructor
	D3DTextureResourceExtraData9();

	/// Virtual destructor
	virtual ~D3DTextureResourceExtraData9();

	/// Returns a string describing the extra this extra data
	virtual std::string ToString();

	/// Return a pointer to the texture object
	const LPDIRECT3DTEXTURE9 GetTexture();

protected:
	/// Pointer to the D3D9 texture object
	LPDIRECT3DTEXTURE9 m_pTexture;
};


/**
	Extra data that is attached to a d3d11 texture resource. This includes
	the actual D3D11 texture object (a shader resource view) and a sampler state.
*/
class D3DTextureResourceExtraData11 : public IResourceExtraData
{
	friend class D3DTextureResourceLoader;

public:
	/// Default constructor
	D3DTextureResourceExtraData11();

	/// Virtual destructor
	virtual ~D3DTextureResourceExtraData11();

	/// Returns a string describing the extra this extra data
	virtual std::string ToString();

	/// Return a pointer to the shader resource view that holds the texture
	ID3D11ShaderResourceView** GetTexture();

	/// Return a pointer to the sampler state for the texture
	ID3D11SamplerState** GetSampler();

protected:
	/// Pointer to the D3D11 shader resource view holding the texture
	ID3D11ShaderResourceView* m_pTexture;

	/// State of the sampler for this texture
	ID3D11SamplerState* m_pSamplerLinear;
};
