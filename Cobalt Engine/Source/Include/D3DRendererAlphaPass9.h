/*
	D3DRendererAlphaPass9.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "interfaces.h"
#include "Matrix.h"

/**
	Helper object used for rendering an alpha pass.
*/

class D3DRendererAlphaPass9 : public IRenderState
{
public:
	/// Default constructor
	D3DRendererAlphaPass9();

	/// Default destructor
	~D3DRendererAlphaPass9();

	/// Return a string describing the object
	std::string ToStr() { return "D3DRendererAlphaPass9"; }

protected:
	Mat4x4 m_OldWorld;
	DWORD m_OldZWriteEnable;
};


D3DRendererAlphaPass9::D3DRendererAlphaPass9()
{
	// store the old state of the renderer
	DXUTGetD3D9Device()->GetTransform(D3DTS_WORLD, &m_OldWorld);
	DXUTGetD3D9Device()->GetRenderState(D3DRS_ZWRITEENABLE, &m_OldZWriteEnable);

	// set the new state for the alpha pass
	DXUTGetD3D9Device()->SetRenderState(D3DRS_ZWRITEENABLE, false);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

D3DRendererAlphaPass9::~D3DRendererAlphaPass9()
{
	// restore the original renderer state
	DXUTGetD3D9Device()->SetRenderState(D3DRS_COLORVERTEX, false);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_ZWRITEENABLE, m_OldZWriteEnable);
	DXUTGetD3D9Device()->SetTransform(D3DTS_WORLD, &m_OldWorld);
}
