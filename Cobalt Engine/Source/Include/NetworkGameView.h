/*
	NetworkGameView.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "EngineStd.h"
#include "interfaces.h"

#define INVALID_SOCKET_ID (-1)

/**
	This class manages a game view that actually exists across the
	network. It tricks a server into thinking the game view is local,
	but forwards the events across the network.
*/
class NetworkGameView : public IGameView
{
public:
	/// Default constructor
	NetworkGameView();

	/// Delegate method to listen for new game object events
	void NewGameObjectDelegate(IEventPtr pEvent);

	/// Set the id of the game object this view is attached to
	void SetPlayerObjectId(GameObjectId gameObjectId) { m_GameObjectId = gameObjectId; }

	/// Called by the game logic when new remote views are added
	void AttachRemotePlayer(int sockID);

	/// Return whether or not the view is attached to a socket
	int HasRemotePlayerAttached() { return m_SockId != INVALID_SOCKET_ID; }

	// IGameView interface
	virtual HRESULT OnRestore() { return S_OK; }
	virtual void OnRender(float time, float deltaTime) { }
	virtual HRESULT OnLostDevice() { return S_OK; }
	virtual GameViewType GetType() { return GameView_Remote; }
	virtual GameViewId GetId() const { return m_ViewId; }
	virtual void OnAttach(GameViewId viewId, GameObjectId objectId);
	virtual LRESULT CALLBACK OnMsgProc(AppMsg msg) { return 0; }
	virtual void OnUpdate(float deltaTime);

private:
	/// Stores the id of the current view
	GameViewId m_ViewId;

	/// Stores the Game Object id if a view is attached to a specific object
	GameObjectId m_GameObjectId;

	/// Id of the socket that this view represents
	int m_SockId;
};
