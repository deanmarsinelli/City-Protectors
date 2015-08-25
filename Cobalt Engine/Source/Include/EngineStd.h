/*
	EngineStd.h

	This file contains extern declarations for various
	global data that is used throughout the engine.

	This file is not platform specific and contains ifdefs
	for the various application layers that the engine
	supports.
*/

#pragma once

#include <d3dx9.h>
#include <FastDelegate.h>
#include <memory>
#include <tinyxml.h>

#include "interfaces.h"

#define CB_SAFE_DELETE(p) { if (p) { delete (p); (p) = nullptr; } }
#define CB_SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p) = nullptr; } }
#define CB_COM_RELEASE(p) { if (p) { p->Release(); (p) = nullptr; } }


#ifdef _DEBUG
 #define CB_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
 #define CB_NEW new
#endif

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
	#include "WindowsApp.h"
	// declaration for the WindowsAppMain function
	extern int WINAPI WindowsAppMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd);
#endif

#ifdef _XBOX
// xbox entry function, etc
#endif

extern const float CB_PI;
extern const float CB_2PI;

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

extern const int MEGABYTE;

extern const float fOPAQUE;
extern const int iOPAQUE;
extern const float fTRANSPARENT;
extern const int iTRANSPARENT;

extern Color g_White;
extern Color g_Black;
extern Color g_Cyan;
extern Color g_Red;
extern Color g_Green;
extern Color g_Blue;
extern Color g_Yellow;
extern Color g_Gray40;
extern Color g_Gray25;
extern Color g_Gray65;
extern Color g_Transparent;
