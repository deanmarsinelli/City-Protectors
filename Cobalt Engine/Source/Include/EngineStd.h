/*
	EngineStd.h

	This file contains extern declarations for various
	global data that is used throughout the engine.

	This file is not platform specific and contains ifdefs
	for the various application layers that the engine
	supports.
*/

#pragma once

#include <tinyxml.h>


#define CB_SAFE_DELETE(p) { if (p) { delete (p); (p) = nullptr; } }
#define CB_SAFE_DELETE_ARRAY(p) { if (p) { delete[](p); (p) = nullptr; } }
#define CB_COM_RELEASE(p) { if (p) { p->Release(); (p) = nullptr; } }


#ifdef _DEBUG
 #define CB_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
 #define CB_NEW new
#endif

typedef D3DXCOLOR Color;

struct AppMsg
{
	HWND m_hWnd;
	UINT m_uMsg;
	WPARAM m_wParam;
	LPARAM m_lParam;
};

//======================================
//  Windows Specific
//======================================
#ifdef _WIN32
 #define DIRECTX
 #include <Windows.h>
 #include "WindowsApp.h"
// declaration for the WindowsAppMain function
extern int WINAPI WindowsAppMain(HINSTANCE hInstance,
	HINSTANCE prevInstance, LPSTR cmdLine, int showCmd);
#endif

#ifdef _XBOX
// xbox entry function, etc
#endif

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int MEGABYTE;

extern Vec3 g_Up;
extern Vec3 g_Right;
extern Vec3 g_Forward;

extern Vec4 g_Up4;
extern Vec4 g_Right4;
extern Vec4 g_Forward4;
