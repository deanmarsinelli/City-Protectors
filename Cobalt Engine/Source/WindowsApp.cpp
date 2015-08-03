/*
	WindowsApp.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <DXUT.h>
#include "WindowsApp.h"

// global pointer for the engine to the application instance
// this will get set in the constructor of the WindowsApp
// class, but will point to a derived instance that is
// global and instantiated before the application begins
WindowsApp* g_pApp = nullptr;

WindowsApp::WindowsApp()
{
	g_pApp = this;
	//m_pGame = nullptr;
}

HWND WindowsApp::GetHwnd()
{
	return DXUTGetHWND();
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
	}
	return result;
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