/*
	CityProtectorsLogic.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <BaseGameLogic.h>

/**
	This is the main game logic layer for city protectors.
*/
class CityProtectorsLogic : public BaseGameLogic
{
public:
	CityProtectorsLogic();
	virtual ~CityProtectorsLogic();

	// overrides
	virtual void MoveGameObject(const GameObjectId id, const Mat4x4& mat) override;
	virtual void ChangeState(BaseGameState newState) override;
	virtual void AddView(shared_ptr<IGameView> pView, GameObjectId objectId = INVALID_GAMEOBJECT_ID) override;
	
	// event delegates
	void RequestStartGameDelegate(IEventPtr pEvent);
	void RemoteClientDelegate(IEventPtr pEvent);
	void NetworkPlayerObjectAssignmentDelegate(IEventPtr pEvent);
	void EnvironmentLoadedDelegate(IEventPtr pEvent);
	void StartThrustDelegate(IEventPtr pEvent);
	void EndThrustDelegate(IEventPtr pEvent);
	void StartSteerDelegate(IEventPtr pEvent);
	void EndSteerDelegate(IEventPtr pEvent);

protected:
	// Delegate for loading a game
	virtual bool LoadGameDelegate(TiXmlElement* pLevelData);

private:
	/// Register all event listener callbacks to the global event manager
	void RegisterAllDelegates();

	/// Remove all event listeners
	void RemoveAllDelegates();

	/// Create a new network event forwarder and listen for certain events
	void CreateNetworkEventForwarder(const int socketId);

	/// Remove all network event forwarders
	void DestroyAllNetworkEventForwarders();

protected:
	/// List of network event forwarders - handles sending events to remote clients
	std::list<NetworkEventForwarder*> m_NetworkEventForwarders;
};
