/*
	D3DSceneNode.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <DXUT.h>

#include "D3DSceneNode.h"
#include "Logger.h"

HRESULT D3DSceneNode9::Render(Scene* pScene)
{
	m_Properties.GetMaterial().D3DUse9();

	switch (m_Properties.GetAlphaType())
	{
	case AlphaTexture:
		break;

	case AlphaMaterial:
		DXUTGetD3D9Device()->SetRenderState(D3DRS_COLORVERTEX, true);
		DXUTGetD3D9Device()->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
		break;

	case AlphaVertex:
		CB_ASSERT(0 && L"Not implemented!");
		break;
	}

	return S_OK;
}