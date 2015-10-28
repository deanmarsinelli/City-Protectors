/*
	CityProtectorsHumanView.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <HumanView.h>
#include <MovementController.h>
#include <SceneNode.h>

#include "ProtectorController.h"
#include "StandardHUD.h"

/**
	The main human game view during gameplay.
*/
class CityProtectorsHumanView : public HumanView
{
public:
	CityProtectorsHumanView(shared_ptr<IRenderer> renderer);
	virtual ~CityProtectorsHumanView();

	virtual LRESULT CALLBACK OnMsgProc(AppMsg msg) override;
	virtual void RenderText() override;
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnAttach(GameViewId viewId, GameObjectId objectId) override;

	virtual void SetControlledGameObject(GameObjectId objectId) override;
	virtual bool LoadGameDelegate(TiXmlElement* pLevelData) override;

	// event delegates
	void GameplayUiUpdateDelegate(IEventPtr pEvent);
	void SetControlledObjectDelegate(IEventPtr pEvent);

private:
	void RegisterAllDelegates();
	void RemoveAllDelegates();

protected:
	/// Render UI or not
	bool m_ShowUI;

	std::wstring m_GameplayText;

	shared_ptr<ProtectorController> m_pProtectorController;
	shared_ptr<MovementController> m_pFreeCameraController;
	shared_ptr<SceneNode> m_pProtector;
	shared_ptr<StandardHUD> m_StandardHUD;
};
