/*
	D3DRendererSkyBoxPass11.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <D3D11.h>
#include <DXUT.h>

#include "EngineStd.h"
#include "interfaces.h"

/**
	Helper object used for rendering a sky pass.
*/

class D3DRendererSkyBoxPass11 : public IRenderState
{
public:
	/// Default constructor
	D3DRendererSkyBoxPass11();

	/// Default destructor
	~D3DRendererSkyBoxPass11();

	/// Return a string describing the object
	std::string ToStr() { return "D3DRendererSkyBoxPass11"; }

protected:
	ID3D11DepthStencilState* m_pOldDepthStencilState;
	ID3D11DepthStencilState* m_pSkyboxDepthStencilState;
};


D3DRendererSkyBoxPass11::D3DRendererSkyBoxPass11()
{
	// create a description of the skybox stencil pass
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = FALSE;
	DXUTGetD3D11Device()->CreateDepthStencilState(&dsDesc, &m_pSkyboxDepthStencilState);
	DXUT_SetDebugName(m_pSkyboxDepthStencilState, "SkyboxDepthStencil");

	UINT stencilRef;
	// store the old stencil state
	DXUTGetD3D11DeviceContext()->OMGetDepthStencilState(&m_pOldDepthStencilState, &stencilRef);

	// set the new sky stencil state
	DXUTGetD3D11DeviceContext()->OMSetDepthStencilState(m_pSkyboxDepthStencilState, 0);
}

D3DRendererSkyBoxPass11::~D3DRendererSkyBoxPass11()
{
	// restore the original stencil state
	DXUTGetD3D11DeviceContext()->OMSetDepthStencilState(m_pOldDepthStencilState, 0);

	CB_COM_RELEASE(m_pOldDepthStencilState);
	CB_COM_RELEASE(m_pSkyboxDepthStencilState);
}
