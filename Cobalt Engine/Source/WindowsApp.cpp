/*
	WindowsApp.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "WindowsApp.h"

#include <DXUT.h>

#include "BaseGameLogic.h"

// global pointer for the engine to the application instance
// this will get set in the constructor of the WindowsApp
// class, but will point to a derived instance that is
// global and instantiated before the application begins
WindowsApp* g_pApp = nullptr;

WindowsApp::WindowsApp()
{
	// TODO finish this
	g_pApp = this;
	m_pGame = nullptr;

	m_RCDesktop.top = m_RCDesktop.left = m_RCDesktop.bottom = m_RCDesktop.right = 0;
	m_ScreenSize = Point(0, 0);
	m_ColorDepth = 32;

	m_IsRunning = false;
	m_IsEditorRunning = false;
	
	m_HasModalDialog = false;
}

inline HWND WindowsApp::GetHwnd()
{
	return DXUTGetHWND();
}

inline HINSTANCE WindowsApp::GetInstance()
{
	return m_hInstance;
}

bool WindowsApp::InitInstance(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd, int screenWidth, int screenHeight)
{
	// check for an existing instance of the game
#ifndef _DEBUG
	if (!IsOnlyInstance(GetGameTitle()))
		return false;
#endif

	// turn off mouse cursor
	SetCursor(nullptr);

	// check for system resources
	bool resourceCheck = false;
	while (!resourceCheck)
	{
		// un-hardcode these in the future if you want to use them appropriately
		const DWORDLONG physicalRAM = 512 * MEGABYTE;
		const DWORDLONG virtualRAM = 1024 * MEGABYTE;
		const DWORDLONG diskSpace = 10 * MEGABYTE;

		if (!CheckStorage(diskSpace))
			return false;

		if (!CheckMemory(physicalRAM, virtualRAM))
			return false;

		const DWORD minCpuSpeed = 1300;	// 1.3Ghz
		DWORD thisCPU = ReadCPUSpeed();
		if (thisCPU < minCpuSpeed)
		{
			// _ERROR("GetCPUSpeed reports CPU is too slow for this game.");
			return false;
		}

		resourceCheck = true;
	}

	m_hInstance = hInstance;

	// register events
	RegisterEngineEvents();
	RegisterGameEvents();

	// initialize resource cache





	// DirectX initialization
	DXUTInit(true, true, lpCmdLine, true);
	if (hWnd == nullptr)
	{
		DXUTCreateWindow(GetGameTitle(), hInstance, GetIcon());
	}
	else
	{
		DXUTSetWindow(hWnd, hWnd, hWnd);
	}

	if (!GetHwnd())
	{
		// error
		return false;
	}
	SetWindowText(GetHwnd(), GetGameTitle());

	// TODO: save game directory

	m_ScreenSize = Point(screenWidth, screenHeight);
	// create the d3d device
	DXUTCreateDevice(D3D_FEATURE_LEVEL_9_3, true, screenWidth, screenHeight);
	
	/*if (GetRendererImpl() == Renderer_D3D9)
	{
		m_Renderer = std::shared_ptr<IRenderer>(new D3DRenderer9());
	}
	else if (GetRendererImpl() == Renderer_D3D11)
	{
		m_Renderer = std::shared_ptr<IRenderer>(new D3DRenderer11());
	}
	m_Renderer->SetBackgroundColor(255, 20, 20, 200);
	m_Renderer->OnRestore();
	*/
	 
	m_pGame = CreateGameAndView();
	if (!m_pGame)
		return false;
	

	return true;
}
static LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pDoneProcessing, void* pUserContext)
{
	LRESULT result = 0;
	switch (uMsg)
	{
	case WM_CLOSE:
		result = g_pApp->OnClose();
		break;

	// foward these windows messages to the game views
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case MM_JOY1BUTTONDOWN:
	case MM_JOY1BUTTONUP:
	case MM_JOY1MOVE:
	case MM_JOY1ZMOVE:
	case MM_JOY2BUTTONDOWN:
	case MM_JOY2BUTTONUP:
	case MM_JOY2MOVE:
	case MM_JOY2ZMOVE:
	{
		if (g_pApp->m_pGame)
		{
			GameViewList viewList = g_pApp->m_pGame->GameViewList();
			AppMsg msg;
			msg.m_hWnd = hWnd;
			msg.m_uMsg = uMsg;
			msg.m_wParam = wParam;
			msg.m_lParam = lParam;
			// iterate the view list in reverse, forward the message to the view that is on top (last in the list)
			for (GameViewList::reverse_iterator it = viewList.rbegin(); it != viewList.rend(); ++it)
			{
				// if the view returns true, it has consumed this message
				if ((*it)->OnMsgProc(msg))
				{
					result = true;
					break;
				}
			}
		}
		break;
	}
	}
	return result;
}

inline bool WindowsApp::HasModalDialog()
{
	return m_HasModalDialog;
}

inline void WindowsApp::ForceModalExit()
{
	PostMessage(GetHwnd(), g_MsgEndModal, 0, g_QuitNoPrompt);
}

LRESULT WindowsApp::OnDisplayChange(int colorDepth, int width, int height)
{
	m_RCDesktop.left = 0;
	m_RCDesktop.top = 0;
	m_RCDesktop.right = width;
	m_RCDesktop.bottom = height;
	m_ColorDepth = colorDepth;

	return 0;
}

LRESULT WindowsApp::OnPowerBroadcast(int event)
{
	if (event == PBT_APMQUERYSUSPEND)
	{
		return BROADCAST_QUERY_DENY;
	}
	else if (event == PBT_APMBATTERYLOW)
	{
		AbortGame();
	}

	return true;
}

LRESULT WindowsApp::OnSysCommand(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case SC_MAXIMIZE:
	{
		if (m_WindowedMode && m_IsRunning)
		{
			// Maximize into full screen
			OnAltEnter();
		}
		return 0;
	}
	case SC_CLOSE:
	{
		if (lParam != g_QuitNoPrompt)
		{
			if (m_QuitRequested)
				return true;

			m_QuitRequested = true;

			if (MessageBox::Ask(QUESTION_QUIT_GAME) == IDNO)
			{
				m_QuitRequested = false;

				return true;
			}
						 
			m_Quitting = true;

			if (HasModalDialog())
			{
				ForceModalExit();

				PostMessage(GetHwnd(), WM_SYSCOMMAND, SC_CLOSE, g_QuitNoPrompt);

				m_QuitRequested = false;
				return true;
			}
			m_QuitRequested = false;

		}
		return 0;
	}
	default:
		return DefWindowProc(GetHwnd(), WM_SYSCOMMAND, wParam, lParam);
	}

	return 0;
}

LRESULT WindowsApp::OnClose()
{
	CB_SAFE_DELETE(m_pGame);
	DestroyWindow(GetHwnd());
	DestroyNetworkEventForwarder();
	
	// TODO: finish this function

	return 0;
}

bool WindowsApp::LoadStrings(std::string language)
{
	std::string languageFile = "Strings\\";
	languageFile += language;
	languageFile += ".xml";

	/*TiXmlElement* pRoot = XmlResourceLoader::LoadAndReturnRootXmlElement(languageFile.c_str());
	if (!pRoot)
	{
		//CB_ERROR("String file is missing");
		return false;
	}*/

	// TODO: finish this function
}