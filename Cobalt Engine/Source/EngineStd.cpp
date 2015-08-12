/*
	EngineStd.cpp

	This file contains definitions for various
	global data that is used throughout the engine.
*/

#include "EngineStd.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int MEGABYTE = 1024 * 1024;


Vec3 g_Right(1.0f, 0.0f, 0.0f);
Vec3 g_Up(0.0f, 1.0f, 0.0f);
Vec3 g_Forward(0.0f, 0.0f, 1.0f);


Vec4 g_Up4(g_Up.x, g_Up.y, g_Up.z, 0.0f);
Vec4 g_Right4(g_Right.x, g_Right.y, g_Right.z, 0.0f);
Vec4 g_Forward4(g_Forward.x, g_Forward.y, g_Forward.z, 0.0f);