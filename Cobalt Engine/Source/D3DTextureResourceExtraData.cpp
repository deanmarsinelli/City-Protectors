/*
	D3DTextureResourceExtraData.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "D3DTextureResourceExtraData.h"

#include "EngineStd.h"

//====================================================
//	D3D9 extra data
//====================================================
D3DTextureResourceExtraData9::D3DTextureResourceExtraData9() :
m_pTexture(nullptr)
{
}

D3DTextureResourceExtraData9::~D3DTextureResourceExtraData9()
{
	CB_COM_RELEASE(m_pTexture);
}

std::string D3DTextureResourceExtraData9::ToString()
{
	return "D3DTextureResourceExtraData9";
}

const LPDIRECT3DTEXTURE9 D3DTextureResourceExtraData9::GetTexture()
{
	return m_pTexture;
}


//====================================================
//	D3D11 extra data
//====================================================
D3DTextureResourceExtraData11::D3DTextureResourceExtraData11() :
m_pTexture(nullptr), m_pSamplerLinear(nullptr)
{
}

D3DTextureResourceExtraData11::~D3DTextureResourceExtraData11()
{
	CB_COM_RELEASE(m_pTexture);
	CB_COM_RELEASE(m_pSamplerLinear);
}

std::string D3DTextureResourceExtraData11::ToString()
{
	return "D3DTextureResourceExtraData11";
}

const ID3D11ShaderResourceView** D3DTextureResourceExtraData11::GetTexture()
{
	return &m_pTexture;
}

const ID3D11SamplerState** D3DTextureResourceExtraData11::GetSampler()
{
	return &m_pSamplerLinear;
}
