/*
	EngineStd.h

	This file contains extern declarations for various
	global data that is used throughout the engine.

	This file is not platform specific and contains ifdefs
	for the various application layers that the engine
	supports.
*/

#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

#include <tinyxml.h>


#define CB_SAFE_DELETE(p) { if (p) { delete (p); (p) = nullptr; } }
#define CB_SAFE_DELETE_ARRAY(p) { if (p) { delete[](p); (p) = nullptr; } }
#define CB_COM_RELEASE(p) { if (p) { p->Release(); (p) = nullptr; } }


#ifdef _DEBUG
 #define CB_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
 #define CB_NEW new
#endif


// declare engine application layer entry function
#ifdef _WIN32
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