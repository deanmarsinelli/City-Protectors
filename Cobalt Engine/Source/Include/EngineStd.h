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

#include "tinyxml.h"

#define CB_SAFE_DELETE(p) { if (p) { delete (p); (p) = NULL; } }
#define CB_COM_RELEASE(p) { if (p) { p->Release(); (p) = NULL; } }

// declare engine application layer entry function
#ifdef _WIN32
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