/*
	WinMain.cpp

	Entry-point of the Windows Application Layer
*/

#include <DXUT.h>
#include <Windows.h>

#include "Logger.h"
#include "EngineStd.h"

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")

// include for heap debugging
#ifdef _DEBUG
#include <crtdbg.h>
#endif

/**
	WindowsAppMain provides the entry point into the engine.
*/
INT WINAPI WindowsAppMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	LPWSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// Initialize logging system
	Logger::Init("logging.xml");

	// Initialize User Options
	g_pApp->m_Options.Init("PlayerOptions.xml", cmdLine);

	// Setting up DirectX callbacks
	DXUTSetCallbackMsgProc(WindowsApp::MsgProc);
	DXUTSetCallbackFrameMove(WindowsApp::OnUpdate);
	DXUTSetCallbackDeviceChanging(WindowsApp::ModifyDeviceSettings);

	// Set the correct callbacks depending on D3D9 or D3D11 renderer
	if (g_pApp->m_Options.m_Renderer == "Direct3D 11")
	{
		DXUTSetCallbackD3D11DeviceAcceptable(WindowsApp::IsD3D11DeviceAcceptable);
		DXUTSetCallbackD3D11DeviceCreated(WindowsApp::OnD3D11CreateDevice);
		DXUTSetCallbackD3D11SwapChainResized(WindowsApp::OnD3D11ResizedSwapChain);
		DXUTSetCallbackD3D11SwapChainReleasing(WindowsApp::OnD3D11ReleasingSwapChain);
		DXUTSetCallbackD3D11DeviceDestroyed(WindowsApp::OnD3D11DestroyDevice);
		DXUTSetCallbackD3D11FrameRender(WindowsApp::OnD3D11FrameRender);
	}
	else if (g_pApp->m_Options.m_Renderer == "Direct3D 9")
	{
		DXUTSetCallbackD3D9DeviceAcceptable(WindowsApp::IsD3D9DeviceAcceptable);
		DXUTSetCallbackD3D9DeviceCreated(WindowsApp::OnD3D9CreateDevice);
		DXUTSetCallbackD3D9DeviceReset(WindowsApp::OnD3D9ResetDevice);
		DXUTSetCallbackD3D9DeviceLost(WindowsApp::OnD3D9LostDevice);
		DXUTSetCallbackD3D9DeviceDestroyed(WindowsApp::OnD3D9DestroyDevice);
		DXUTSetCallbackD3D9FrameRender(WindowsApp::OnD3D9FrameRender);
	}
	else
	{
		CB_ASSERT(0 && "Unknown renderer specified in game options");
		return false;
	}

	// Show cursor
	DXUTSetCursorSettings(true, true);

	// Init the application
	if (!g_pApp->InitInstance(hInstance, cmdLine, 0, g_pApp->m_Options.m_ScreenSize.x, g_pApp->m_Options.m_ScreenSize.y))
	{
		// FUTURE release memory and show an error message
		return false;
	}

	// Main game loop
	DXUTMainLoop();
	DXUTShutdown();

	// destroy logger
	Logger::Destroy();

	return g_pApp->GetExitCode();
}
