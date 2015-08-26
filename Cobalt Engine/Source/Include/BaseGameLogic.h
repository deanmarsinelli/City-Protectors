/*
	BaseGameLogic.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <unordered_map>

#include "GameObjectFactory.h"
#include "interfaces.h"
#include "MathUtils.h"
#include "Process.h"

typedef std::unordered_map<GameObjectId, StrongGameObjectPtr> GameObjectMap;

enum BaseGameState
{
	Invalid,
	Initializing,
	MainMenu,
	WaitingForPlayers,
	LoadingGameEnvironment,
	WaitingForPlayersToLoadEnvironment,
	SpawningPlayersObjects,
	Running
};

class IGamePhysics;
class PathingGraph;
class LevelManager;

/*
	This is the base class for a game's logic layer. It inherits from the
	game logic interface and is supposed to be subclassed by specific game
	logic implementations.
*/
class BaseGameLogic : public IGameLogic
{
public:	
	/// Default logic constructor
	BaseGameLogic();

	/// Virtual destructor
	virtual ~BaseGameLogic();

	// Initialize the game logic
	bool Init();

	void SetProxy(bool isProxy);

	/// Is this logic a proxy logic?
	const bool IsProxy() const;

	/// Can the logic layer run lua scripts?
	const bool CanRunLua() const;

	/// Return the list of game views attached to this logic
	GameViewList& GetGameViewList();
	
	/// Return the next object id
	GameObjectId GetNewObjectId();

	/// Return the pathing graph for this logic
	shared_ptr<PathingGraph> GetPathingGraph();

	/// Return the random number generator
	CBRandom& GetRNG();

	/// Attach a view to the game logic
	virtual void AddView(shared_ptr<IGameView> pView, GameObjectId id = INVALID_GAMEOBJECT_ID);

	/// Remove a view from the game logic
	virtual void RemoveView(shared_ptr<IGameView> pView);

	/// Return a weak pointer to a game object
	virtual WeakGameObjectPtr GetGameObject(const GameObjectId id);

	/// Create a game object
	virtual StrongGameObjectPtr CreateGameObject(const std::string& objectResource, TiXmlElement* overrides,
		const Mat4x4* initialTransform = nullptr, const GameObjectId serversObjectId = INVALID_GAMEOBJECT_ID);

	/// Destroy a game object
	virtual void DestroyGameObject(const GameObjectId id);

	/// Modify a game object
	virtual void ModifyGameObject(const GameObjectId id, TiXmlElement* overrides);

	/// Move a game object -- override this
	virtual void MoveGameObject(const GameObjectId id, const Mat4x4& mat) { }

	/// Get game object xml (for the editor)
	std::string GetGameObjectXml(const GameObjectId id);

	/// Load a game from a resource
	virtual bool LoadGame(const char* levelResource) override;

	/// Return a pointer to the level manager
	const LevelManager* GetLevelManager();

	/// Set whether this is a proxy logic or not
	virtual void SetProxy();

	/// Update the game logic -- called once per frame
	virtual void OnUpdate(float time, float deltaTime);

	/// Change the state of the game logic
	virtual void ChangeState(enum BaseGameState newState);

	/// Return the state of the game logic
	const BaseGameState GetState() const;

	/// Toggle on/off rendering of diagnostics
	void ToggleRenderDiagnostics();
	
	/// Render physics diagnostics
	void RenderDiagnostics();

	/// Return a pointer to the game physics
	virtual shared_ptr<IGamePhysics> GetGamePhysics();

	/// Attach a process to the game logic
	void AttachProcess(StrongProcessPtr pProcess);

	/// Event delegate for destroying a game object
	void RequestDestroyGameObjectDelegate(IEventPtr pEvent);

protected:
	/// Create a game object factory
	virtual GameObjectFactory* CreateObjectFactory();

	/// Load game delegate, override this for game-specific loading
	virtual bool LoadGameDelegate(TiXmlElement* pLevelDate);

	/// Event delegate for moving a game object
	void MoveGameObjectDelegate(IEventPtr pEvent);

	/// Event delegate for creating a new game object
	void RequestNewGameObjectDelegate(IEventPtr pEvent);

protected:
	/// How long this game has been in session
	float m_LifeTime;
	
	/// Process manager for the logic
	ProcessManager* m_pProcessManager;

	/// Random number generator
	CBRandom m_Random;

	/// Map of game objects in this logic
	GameObjectMap m_Objects;

	/// The id of the last game object created
	GameObjectId m_LastObjectId;

	/// Game state: loading, running, etc
	BaseGameState m_State;

	/// How many local players we expect
	int m_ExpectedPlayers;

	/// How many remote players we expect
	int m_ExpectedRemotePlayers;

	/// How many AI players we expect
	int m_ExpectedAI;

	/// How many human players are attached to the logic
	int m_HumanPlayersAttached;

	/// How many AI players are attached to the logic
	int m_AIPlayersAttached;

	/// How many of the human players are loaded
	int m_HumanGamesLoaded;

	/// Views that are attached to our game
	GameViewList m_GameViews;

	/// The pathing graph for AI's
	shared_ptr<PathingGraph> m_pPathingGraph;

	/// Factory for creating new game objects
	GameObjectFactory* m_pObjectFactory;

	/// Is this a proxy game logic? or a real one
	bool m_Proxy;

	/// If this is a remote player, what is their socket number on the server
	int m_RemotePlayerId;

	/// Are we rendering diagnostics?
	bool m_RenderDiagnostics;

	/// Pointer to the game physics
	shared_ptr<IGamePhysics> m_pPhysics;

	/// Manages loading and chaining levels
	LevelManager* m_pLevelManager;
};
