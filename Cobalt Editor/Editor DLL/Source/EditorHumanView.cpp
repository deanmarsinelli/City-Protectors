/*
	EditorHumanView.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "EditorHumanView.h"

EditorHumanView::EditorHumanView(shared_ptr<IRenderer> renderer) :
HumanView(renderer)
{
	// no UI classes, any level info should be displayed in C#
}

void EditorHumanView::OnUpdate(float deltaTime)
{
	HumanView::OnUpdate(deltaTime);

	if (m_pFreeCameraController)
	{
		m_pFreeCameraController->OnUpdate(deltaTime);
	}
}

shared_ptr<ScreenElementScene> EditorHumanView::GetScene()
{
	return m_pScene;
}

bool EditorHumanView::LoadGameDelegate(TiXmlElement* pLevelData)
{
	if (!HumanView::LoadGameDelegate(pLevelData))
	{
		return false;
	}

	// hook up the movement controller to keyboard / mouse
	m_pFreeCameraController.reset(CB_NEW MovementController(m_pCamera, 90, 0, true));
	m_pCamera->ClearTarget();

	m_KeyboardHandler = m_pFreeCameraController;
	m_PointerHandler = m_pFreeCameraController;

	m_pScene->OnRestore();

	return true;
}
