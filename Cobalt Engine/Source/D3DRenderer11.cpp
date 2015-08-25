/*
	D3DRenderer11.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "D3DRenderer11.h"

#include "EngineStd.h"

D3DRenderer11::D3DRenderer11()
{
	m_pLineDrawer = nullptr;
}

HRESULT D3DRenderer11::OnRestore()
{
	HRESULT hr;
	V_RETURN(D3DRenderer::OnRestore());
	CB_SAFE_DELETE(D3DRenderer::g_pTextHelper);

	D3DRenderer::g_pTextHelper = CB_NEW CDXUTTextHelper(DXUTGetD3D11Device(), DXUTGetD3D11DeviceContext(), &g_DialogResourceManager, 15);
	return S_OK;
}
