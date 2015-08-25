/*
	MessageBox.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "MessageBox.h"

#include <algorithm>

#include "EngineStd.h"
#include "D3DRenderer.h"
#include "Logger.h"

// static method definitions
void CALLBACK CBMessageBox::OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext)
{
	PostMessage(g_pApp->GetHwnd(), g_MsgEndModal, 0, nControlID);
}

int CBMessageBox::Ask(MessageBox_Questions question)
{
	// TODO
	return 0;
}

// method definitions
CBMessageBox::CBMessageBox(std::wstring message, std::wstring title, int buttonFlags)
{
	// initialize directx dialogs
	m_UI.Init(&D3DRenderer::g_DialogResourceManager);
	m_UI.SetCallback(OnGUIEvent);

	// get dimensions of the message
	RECT rc;
	SetRect(&rc, 0, 0, 0, 0);
	m_UI.CalcTextRect(message.c_str(), m_UI.GetDefaultElement(DXUT_CONTROL_STATIC, 0), &rc);
	int messageWidth = rc.right - rc.left;
	int messageHeight = rc.bottom - rc.top;

	// set number of buttons based on button flags, default is 2
	int nButtons = 2;
	if (buttonFlags == MB_ABORTRETRYIGNORE || buttonFlags == MB_CANCELTRYCONTINUE)
	{
		nButtons = 3;
	}
	else if (buttonFlags == MB_OK)
	{
		nButtons = 1;
	}

	// get dimensions for the message box
	int buttonWidth = (int)((float)g_pApp->GetScreenSize().x * 0.15f);
	int buttonHeight = (int)((float)g_pApp->GetScreenSize().y * 0.037f);
	int border = (int)((float)g_pApp->GetScreenSize().x * 0.043f);

	m_Width = max(messageWidth + 2 * border, buttonWidth + 2 * border);
	m_Height = messageHeight + (nButtons * (buttonHeight + border)) + (2 * border);
	m_UI.SetSize(m_Width, m_Height);

	// set the message box position
	m_PosX = (g_pApp->GetScreenSize().x - m_Width) / 2;
	m_PosY = (g_pApp->GetScreenSize().y - m_Height) / 2;
	m_UI.SetLocation(m_PosX, m_PosY);
	
	m_UI.SetBackgroundColors(g_Gray40);

	int iX = (m_Width - messageWidth) / 2;
	int iY = border;

	m_UI.AddStatic(0, message.c_str(), iX, iY, messageWidth, messageHeight);

	iX = (m_Width - buttonWidth) / 2;
	iY = m_Height - buttonHeight - border;
	
	// set up the actual message box with buttons
	buttonFlags &= 0xF;
	if (buttonFlags == MB_ABORTRETRYIGNORE || buttonFlags == MB_CANCELTRYCONTINUE)
	{
		// message box contains 3 buttons: continue, try again, cancel
		m_UI.AddButton(IDCONTINUE, g_pApp->GetString(L"IDS_CONTINUE").c_str(), iX, iY - (2 * border), buttonWidth, buttonHeight);
		m_UI.AddButton(IDTRYAGAIN, g_pApp->GetString(L"IDS_TRYAGAIN").c_str(), iX, iY - border, buttonWidth, buttonHeight);
		m_UI.AddButton(IDCANCEL, g_pApp->GetString(L"IDS_CANCEL").c_str(), iX, iY, buttonWidth, buttonHeight);
	}
	else if (buttonFlags == MB_OKCANCEL)
	{
		// contains 2 buttons: OK and cancel
		m_UI.AddButton(IDOK, g_pApp->GetString(L"IDS_OK").c_str(), iX, iY - border, buttonWidth, buttonHeight);
		m_UI.AddButton(IDCANCEL, g_pApp->GetString(L"IDS_CANCEL").c_str(), iX, iY, buttonWidth, buttonHeight);
	}
	else if (buttonFlags == MB_RETRYCANCEL)
	{
		// contains 2 buttons: retry and cancel
		m_UI.AddButton(IDRETRY, g_pApp->GetString(L"IDS_RETRY").c_str(), iX, iY - border, buttonWidth, buttonHeight);
		m_UI.AddButton(IDCANCEL, g_pApp->GetString(L"IDS_CANCEL").c_str(), iX, iY, buttonWidth, buttonHeight);
	}
	else if (buttonFlags == MB_YESNO)
	{
		// contains 2 buttons: yes and no
		m_UI.AddButton(IDYES, g_pApp->GetString(L"IDS_YES").c_str(), iX, iY - border, buttonWidth, buttonHeight, 0x59);
		m_UI.AddButton(IDNO, g_pApp->GetString(L"IDS_NO").c_str(), iX, iY, buttonWidth, buttonHeight, 0x4E);
	}
	else if (buttonFlags == MB_YESNOCANCEL)
	{
		// message box contains 3 buttons: yes, no, cancel
		m_UI.AddButton(IDYES, g_pApp->GetString(L"IDS_YES").c_str(), iX, iY - (2 * border), buttonWidth, buttonHeight);
		m_UI.AddButton(IDNO, g_pApp->GetString(L"IDS_NO").c_str(), iX, iY - border, buttonWidth, buttonHeight);
		m_UI.AddButton(IDCANCEL, g_pApp->GetString(L"IDS_CANCEL").c_str(), iX, iY, buttonWidth, buttonHeight);
	}
	else
	{
		// default message box has one button: ok
		m_UI.AddButton(IDOK, g_pApp->GetString(L"IDS_OK").c_str(), iX, iY, buttonWidth, buttonHeight);
	}
}

CBMessageBox::~CBMessageBox()
{
	CB_ASSERT(1);
}

HRESULT CBMessageBox::OnRestore()
{
	m_UI.SetLocation(m_PosX, m_PosY);
	m_UI.SetSize(m_Width, m_Height);
	return S_OK;
}

HRESULT CBMessageBox::OnRender(float time, float deltaTime)
{
	HRESULT hr;
	V(m_UI.OnRender(deltaTime));
	return S_OK;
}

int CBMessageBox::GetZOrder() const
{
	return 99;
}

LRESULT CALLBACK CBMessageBox::OnMsgProc(AppMsg msg)
{
	return m_UI.MsgProc(msg.m_hWnd, msg.m_uMsg, msg.m_wParam, msg.m_lParam);
}
