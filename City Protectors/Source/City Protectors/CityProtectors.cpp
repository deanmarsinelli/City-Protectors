#include <EngineStd.h>

#include "CityProtectors.h"
#include "CityProtectorsLogic.h"
#include "GameEvents.h"
#include "MainMenuView.h"

// global object for the game
// this will be instantiated before main() and the constructor
// will set the global pointer in the engine to this object
// allowing the engine access to our derived game class
CityProtectors g_Game;

/**
	WinMain is the entry point for the game.
	It forwards the call to the engine's WindowsAppMain function and allows for
	separation between the engine library and the game application.
*/
INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	return WindowsAppMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}


HICON CityProtectors::GetIcon()
{
	// todo: change the icon
	return LoadIcon(GetInstance(), MAKEINTRESOURCE(IDI_WINLOGO));
}

BaseGameLogic* CityProtectors::CreateGameAndView()
{
	// create a new game logic and initialize it
	m_pGame = CB_NEW CityProtectorsLogic();
	m_pGame->Init();

	// create the main menu, add it to the view list
	shared_ptr<IGameView> menuView(CB_NEW MainMenuView());
	m_pGame->AddView(menuView);

	return m_pGame;
}

void CityProtectors::RegisterGameEvents()
{
	REGISTER_EVENT(Event_StartThrust);
	REGISTER_EVENT(Event_EndThrust);
	REGISTER_EVENT(Event_StartSteer);
	REGISTER_EVENT(Event_EndSteer);
}

void CityProtectors::CreateNetworkEventForwarder()
{

}

void CityProtectors::DestroyNetworkEventForwarder()
{

}
