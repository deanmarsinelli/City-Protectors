/*
	StandardHUD.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <D3DRenderer.h>
#include <EngineStd.h>

#include "StandardHUD.h"

#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3

StandardHUD::StandardHUD()
{
	// initialize the DXUT dialog
	m_HUD.Init(&D3DRenderer::g_DialogResourceManager);
	m_HUD.SetCallback(OnGuiEvent);
	int iY = 10;
	m_HUD.AddButton(IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22);
	m_HUD.AddButton(IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22);
}

StandardHUD::~StandardHUD()
{
	// FUTURE - find a way to unregister the m_HUD dialog
}

HRESULT StandardHUD::OnRestore()
{
	m_HUD.SetLocation(g_pApp->GetScreenSize().x - 170, 0);
	m_HUD.SetSize(170, 170);
	return S_OK;
}

HRESULT StandardHUD::OnRender(float time, float deltaTime)
{
	HRESULT hr;

	// tell PIX what is happening
	DXUT_BeginPerfEvent(DXUT_PERFEVENTCOLOR, L"StandardUI");
	V(m_HUD.OnRender(deltaTime));
	DXUT_EndPerfEvent();

	return S_OK;
}

LRESULT CALLBACK StandardHUD::OnMsgProc(AppMsg msg)
{
	// forward OS messages to DXUT dialog
	return m_HUD.MsgProc(msg.m_hWnd, msg.m_uMsg, msg.m_wParam, msg.m_lParam);
}

void CALLBACK StandardHUD::OnGuiEvent(UINT nEvents, int nControlID, CDXUTControl* pControl, void* pUserContext)
{
	switch (nControlID)
	{
	case IDC_TOGGLEFULLSCREEN:
		// set full screen
		DXUTToggleFullScreen();
		break;

	case IDC_TOGGLEREF:
		// toggle HAL or reference device
		DXUTToggleREF();
		break;
	}
}
