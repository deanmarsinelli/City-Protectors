/*
	D3DRendererAlphaPass11.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <D3D11.h>
#include <DXUT.h>

#include "EngineStd.h"
#include "interfaces.h"

/**
	Helper object used for rendering an alpha pass.
*/
class D3DRendererAlphaPass11 : public IRenderState
{
public:
	/// Default constructor
	D3DRendererAlphaPass11();

	/// Default destructor
	~D3DRendererAlphaPass11();

	/// Return a string describing the object
	std::string ToStr() { return "D3DRendererAlphaPass11"; }

protected:
	ID3D11BlendState* m_pCurrentBlendState;
	ID3D11BlendState* m_pOldBlendState;
	FLOAT m_OldBlendFactor[4];
	UINT m_OldSampleMask;
};


D3DRendererAlphaPass11::D3DRendererAlphaPass11()
{
	// store the current blend state
	DXUTGetD3D11DeviceContext()->OMGetBlendState(&m_pOldBlendState, m_OldBlendFactor, &m_OldSampleMask);
	m_pCurrentBlendState = nullptr;

	D3D11_BLEND_DESC blendState;
	ZeroMemory(&blendState, sizeof(D3D11_BLEND_DESC));

	// set up an alpha blend state
	blendState.AlphaToCoverageEnable = false;
	blendState.IndependentBlendEnable = false;
	blendState.RenderTarget[0].BlendEnable = TRUE;
	blendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	DXUTGetD3D11Device()->CreateBlendState(&blendState, &m_pCurrentBlendState);
	DXUTGetD3D11DeviceContext()->OMSetBlendState(m_pCurrentBlendState, 0, 0xffffffff);
}

D3DRendererAlphaPass11::~D3DRendererAlphaPass11()
{
	// restore the original blend state
	DXUTGetD3D11DeviceContext()->OMSetBlendState(m_pOldBlendState, m_OldBlendFactor, m_OldSampleMask);
	CB_COM_RELEASE(m_pCurrentBlendState);
	CB_COM_RELEASE(m_pOldBlendState);
}