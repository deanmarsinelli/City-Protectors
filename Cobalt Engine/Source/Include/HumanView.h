/*
	HumanView.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <tinyxml.h>

#include "BaseGameLogic.h"
#include "Console.h"
#include "interfaces.h"
#include "ProcessManager.h"
#include "UserInterface.h"

/**
	This class represents the Game View layer for a human controlled player. This view
	can be static for a single screen or attached to a particular game object. This class
	should be subclassed for a particular game.
*/
class HumanView : public IGameView 
{
public:
	/// Constructor taking a renderer pointer -- initializes the view
	HumanView(shared_ptr<IRenderer> renderer);

	/// Default destructor
	virtual ~HumanView();
	
	/// Creates view specific elements from an xml file
	bool LoadGame(TiXmlElement* pLevelData);

	/// Initialize the audio system associated with this view
	bool InitAudio();

	// Implement the IGameView interface
	/// Recreates anything that might have been lost while the game is running (because of system sleep, etc). Also used for initialization
	virtual HRESULT OnRestore();

	/// Render the game view
	virtual void OnRender(double time, float deltaTime);

	/// Called when the device is lost
	virtual HRESULT OnLostDevice();

	/// Return the type of the game view
	virtual GameViewType GetType();

	/// Return the id of the game view
	GameViewId GetId() const;

	/// Attach a game object to this view
	virtual void OnAttach(GameViewId viewId, GameObjectId objectId);

	/// Handle messages sent from the OS
	virtual LRESULT CALLBACK OnMsgProc(AppMsg msg);

	/// Update method called once per frame
	virtual void OnUpdate(float deltaTime);

	// methods to control the layering of view elements
	/// Add an element to the view hierarchy
	virtual void PushElement(shared_ptr<IScreenElement> pElement);

	/// Remove an element from the view hierarchy
	virtual void RemoveElement(shared_ptr<IScreenElement> pElement);

	/// Pause or unpause this game view
	void TogglePause(bool active);

	/// Return a pointer to the process manager
	ProcessManager* GetProcessManager();

	/// Set this view to be associated with a particular game object
	virtual void SetControlledGameObject(GameObjectId objectId);

	/// Set a camera offset -- useful for third person camera
	virtual void SetCameraOffset(const Vec4& offset);

	void PlaySoundDelegate(IEventDataPtr pEventData);
	void GameStateDelegate(IEventDataPtr pEventData);

	/// Return a reference to the console
	Console& GetConsole();

protected:
	/// Called when text is needed to be drawn on the screen -- should be overriden
	virtual void RenderText() { };

	virtual bool LoadGameDelegate(TiXmlElement* pLevelData);

private:
	void RegisterAllDelegates();
	void RemoveAllDelegates();

public:
	/// List of UI elements for this view
	ScreenElementList m_ScreenElements;

	/// Handler to the mouse pointer
	shared_ptr<IPointerHandler> m_PointerHandler;

	/// Defines how large of an area the mouse pointer covers
	int m_PointerRadius;

	/// Handler to the keyboard
	shared_ptr<IKeyboardHandler> m_KeyboardHandler;

	shared_ptr<ScreenElementScene> m_pScene;

	/// Pointer to a camera for the view
	shared_ptr<CameraNode> m_pCamera;

protected:
	/// Stores the id of the current view
	GameViewId m_ViewId;

	/// Stores the Game Object id if a view is attached to a specific object
	GameObjectId m_GameObjectId;

	/// Process manager used for things like button animations -- anything that takes multiple frames to execute
	ProcessManager* m_pProcessManager;

	/// Current time
	DWORD m_CurrTick;

	/// Last time the game rendered
	DWORD m_LastDraw;

	/// Should the game run full speed or limit the framerate
	bool m_RunFullSpeed;

	/// Current state of the game
	BaseGameState m_BaseGameState;

	/// Console where the player can input commands
	Console m_Console;
};