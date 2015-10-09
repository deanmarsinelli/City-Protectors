/**
	Game.h

	Game is the main class that runs the
	game and all its logic. It is a derived
	class that inherits from the Engine's
	Application Layer class for whatever the
	target platform is.
*/

#pragma once

#include <WindowsApp.h>

/*
	Main Game class - inherits from the Application Layer class
	of whatever platform the game is running on
*/
class Game : public WindowsApp
{
public:
	virtual TCHAR* GetGameTitle() { return L"Game Title"; }
	virtual TCHAR* GetGameAppDirectory() { return L"SomeDirectory"; }
	virtual HICON  GetIcon();

protected:
	//virtual BaseGameLogic* CreateGameAndView();
	virtual void RegisterGameEvents();
	virtual void 
		();
	virtual void DestroyNetworkEventForwarder();
};