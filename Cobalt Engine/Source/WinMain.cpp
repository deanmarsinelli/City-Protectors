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
	LPSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	MSG msg = { 0 };

	/* MAIN GAME LOOP */
	while (msg.message != WM_QUIT)
	{
		// if theres a message, handle it
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}