/*
	MainMenuView.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <HumanView.h>

#include "MainMenuUI.h"

/**
	This is the view a human player will see during the 
	main menu of the game.
*/
class MainMenuView : public HumanView
{
public:
	MainMenuView();
	~MainMenuView();

	virtual LRESULT CALLBACK OnMsgProc(AppMsg msg);
	virtual void RenderText();
	virtual void OnUpdate(float deltaTime);

protected:
	shared_ptr<MainMenuUI> m_MainMenuUI;
};