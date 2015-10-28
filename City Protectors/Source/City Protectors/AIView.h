/*
	AIView.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <interfaces.h>
#include <EngineStd.h>

class PathingGraph;

class AIView : public IGameView
{
	friend class AIViewListener;

public:
	AIView(shared_ptr<PathingGraph> pPathingGraph);
	virtual ~AIView();

	virtual HRESULT OnRestore() override { return S_OK; }
	virtual void OnRender(float time, float deltaTime) override {}
	virtual HRESULT OnLostDevice() override { return S_OK; }
	virtual GameViewType GetType() override { return GameView_AI; }
	virtual GameViewId GetId() const override { return m_ViewId; }
	virtual void OnAttach(GameViewId viewId, GameObjectId objectId) override { m_ViewId = viewId; m_PlayerObjectId = objectId; }
	virtual LRESULT CALLBACK OnMsgProc(AppMsg msg) override { return 0; }
	virtual void OnUpdate(float deltaTime) override {}

	shared_ptr<PathingGraph> GetPathingGraph(void) const { return m_pPathingGraph; }

protected:
	GameViewId m_ViewId;
	GameObjectId m_PlayerObjectId;

private:
	shared_ptr<PathingGraph> m_pPathingGraph;
};
