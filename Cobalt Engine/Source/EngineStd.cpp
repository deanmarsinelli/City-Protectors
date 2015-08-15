/*
	EngineStd.cpp

	This file contains definitions for various
	global data that is used throughout the engine.
*/

#include "EngineStd.h"

const float CB_PI = 3.14159265358979f;
const float CB_2PI = 2.0f * CB_PI;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int MEGABYTE = 1024 * 1024;

const float fOPAQUE = 1.0f;
const int iOPAQUE = 1;
const float fTRANSPARENT = 0.0f;
const int iTRANSPARENT = 0;

Color g_White(1.0f, 1.0f, 1.0f, fOPAQUE);
Color g_Black(0.0f, 0.0f, 0.0f, fOPAQUE);
Color g_Cyan(0.0f, 1.0f, 1.0f, fOPAQUE);
Color g_Red(1.0f, 0.0f, 0.0f, fOPAQUE);
Color g_Green(0.0f, 1.0f, 0.0f, fOPAQUE);
Color g_Blue(0.0f, 0.0f, 1.0f, fOPAQUE);
Color g_Yellow(1.0f, 1.0f, 0.0f, fOPAQUE);
Color g_Gray40(0.4f, 0.4f, 0.4f, fOPAQUE);
Color g_Gray25(0.25f, 0.25f, 0.25f, fOPAQUE);
Color g_Gray65(0.65f, 0.65f, 0.65f, fOPAQUE);
Color g_Transparent(1.0f, 0.0f, 1.0f, fTRANSPARENT);
