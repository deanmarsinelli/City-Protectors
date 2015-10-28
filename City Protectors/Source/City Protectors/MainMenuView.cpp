/*
	MainMenuView.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "MainMenuView.h"

MainMenuView::MainMenuView() :
HumanView(shared_ptr<IRenderer>())
{
	// create the main menu UI, this view has no 3d renderer
	m_MainMenuUI.reset(CB_NEW MainMenuUI);
	PushElement(m_MainMenuUI);
}

MainMenuView::~MainMenuView()
{
	// nothing to free
}

LRESULT CALLBACK MainMenuView::OnMsgProc(AppMsg msg)
{
	if (m_MainMenuUI->IsVisible())
	{
		if (HumanView::OnMsgProc(msg))
		{
			return 1;
		}
	}
	return 0;
}

void MainMenuView::RenderText()
{
	HumanView::RenderText();
}

void MainMenuView::OnUpdate(float deltaTime)
{
	HumanView::OnUpdate(deltaTime);
}
