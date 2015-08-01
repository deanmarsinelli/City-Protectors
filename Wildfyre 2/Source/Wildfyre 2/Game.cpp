#include "Game.h"
#include "EngineStd.h"

// global object for the game
// this will be instantiated before main() and the constructor
// will set the global pointer in the engine to this object
// allowing the engine access to our derived game class
Game g_Game;

/**
	WinMain is the entry point for the game.
	It forwards the call to the engine's WindowsAppMain function
*/

#ifdef _WIN32
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return WindowsAppMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
#endif

HICON Game::GetIcon()
{
	// todo: change the icon
	return LoadIcon(GetInstance(), MAKEINTRESOURCE(IDI_WINLOGO));
}