/**
	WinMain.cpp

	Entry-point of the Windows Application Layer
*/

#include <DXUT.h>
#include <Windows.h>
#include "WindowsApp.h"

// include for heap debugging
#ifdef _DEBUG
#include <crtdbg.h>
#endif

/**
	WindowsAppMain provides the entry point into the engine.
*/
int WINAPI WindowsAppMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	LPWSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// TODO: Initialize logging system

	// Initialize User Options
	g_pApp->m_Options.Init("UserOptions.xml", cmdLine);

	return g_pApp->GetExitCode();
}