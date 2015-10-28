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

	virtual void SetProxy();
	virtual void MoveObject(const GameObjectId, const Mat4x4& mat);

	// overrides
	virtual void ChangeState(BaseGameState newState) override;
	virtual void AddView(shared_ptr<IGameView> pView, GameObjectId objectId = INVALID_GAMEOBJECT_ID) override;
	virtual shared_ptr<IGamePhysics> GetGamePhysics() override;
	
	// event delegates
	void RequestStartGameDelegate(IEventPtr pEvent);
	void RemoteClientDelegate(IEventPtr pEvent);
	void NetworkPlayerObjectAssignmentDelegate(IEventPtr pEvent);
	void EnvironmentLoadedDelegate(IEventPtr pEvent);
	void ThrustDelegate(IEventPtr pEvent);
	void SteerDelegate(IEventPtr pEvent);
	void StartThrustDelegate(IEventPtr pEvent);
	void EndThrustDelegate(IEventPtr pEvent);
	void StartSteerDelegate(IEventPtr pEvent);
	void EndSteerDelegate(IEventPtr pEvent);

protected:
	// delegate for loading a game
	virtual bool LoadGameDelegate(TiXmlElement* pLevelData);

private:
	void RegisterAllDelegates();
	void RemoveAllDelegates();
	void CreateNetworkEventForwarder(const int socketId);
	void DestroyAllNetworkEventForwarders();

protected:
	std::list<NetworkEventForwarder*> m_NetworkEventForwarders;
};
