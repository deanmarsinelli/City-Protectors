/*
	D3DRenderer.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "D3DRenderer.h"

#include "EngineStd.h"

// initialize static variables
CDXUTDialogResourceManager D3DRenderer::g_DialogResourceManager;
CDXUTTextHelper *D3DRenderer::g_pTextHelper = nullptr;

HRESULT D3DRenderer::OnRestore()
{
	return S_OK;
}

void D3DRenderer::Shutdown()
{
	CB_SAFE_DELETE(g_pTextHelper);
}
