/*
	EditorHumanView.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <HumanView.h>
#include <MovementController.h>

class EditorHumanView : public HumanView
{
public:
	EditorHumanView(shared_ptr<IRenderer> renderer);
	virtual ~EditorHumanView() { }

	virtual void OnUpdate(float deltaTime) override;

	shared_ptr<ScreenElementScene> GetScene();

protected:
	virtual bool LoadGameDelegate(TiXmlElement* pLevelData) override;

protected:
	shared_ptr<MovementController> m_pFreeCameraController;
};
