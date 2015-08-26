/*
	D3DRendererSkyBoxPass9.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "interfaces.h"

/**
	Helper object used for rendering a sky pass.
*/

class D3DRendererSkyBoxPass9 : public IRenderState
{
public:
	/// Default constructor
	D3DRendererSkyBoxPass9();

	/// Default destructor
	~D3DRendererSkyBoxPass9();

	/// Return a string describing the object
	std::string ToStr() { return "D3DRendererSkyBoxPass9"; }

protected:
	DWORD m_OldLightMode;
	DWORD m_OldCullMode;
	DWORD m_OldZWriteEnable;
};


D3DRendererSkyBoxPass9::D3DRendererSkyBoxPass9()
{
	// store current render states
	DXUTGetD3D9Device()->GetRenderState(D3DRS_LIGHTING, &m_OldLightMode);
	DXUTGetD3D9Device()->GetRenderState(D3DRS_ZWRITEENABLE, &m_OldZWriteEnable);
	DXUTGetD3D9Device()->GetRenderState(D3DRS_CULLMODE, &m_OldCullMode);

	// set skybox render states
	DXUTGetD3D9Device()->SetRenderState(D3DRS_LIGHTING, FALSE);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_ZWRITEENABLE, false);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

D3DRendererSkyBoxPass9::~D3DRendererSkyBoxPass9()
{
	DXUTGetD3D9Device()->SetRenderState(D3DRS_LIGHTING, m_OldLightMode);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_CULLMODE, m_OldCullMode);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_ZWRITEENABLE, m_OldZWriteEnable);
}
