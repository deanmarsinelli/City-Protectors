/*
	UserInterface.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "interfaces.h"
#include "Logger.h"
#include "Scene.h"

const DWORD g_QuitNoPrompt = MAKELPARAM(-1, -1);
const UINT g_MsgEndModal = (WM_USER + 100);

/**
	Base class for UI elements. Must be subclassed.
*/
class BaseUI : public IScreenElement
{
public:
	/// Default constructor
	BaseUI() :
		m_IsVisible(true)
	{
		m_PosX = m_PosY = 0;
		m_Width = m_Height = 100;
	}

	/// Update method
	virtual void OnUpdate(float deltaTime) { }

	/// Recreates anything that might have been lost while the game is running (because of system sleep, etc)
	virtual HRESULT OnLostDevice() { return S_OK; }

	/// Return whether or not the element is visible
	virtual bool IsVisible() const { return m_IsVisible; }

	/// Set an element visible or invisible
	virtual void SetVisible(bool visible) { m_IsVisible = visible; }

protected:
	/// X-coordinate of the top left position of the element
	int	m_PosX;

	/// Y-coordinate of the top left position of the element
	int m_PosY;

	/// Width of the element
	int m_Width;
	
	/// Height of the element
	int m_Height;

	/// Dialog result (for buttons)
	int m_Result;

	/// Should the element be rendered?
	bool m_IsVisible;
};


/**
	A basic scene that represents a 3D world + the UI.
*/
class ScreenElementScene : public IScreenElement, public Scene
{
public:
	/// Constructor intializes the scene
	ScreenElementScene(shared_ptr<IRenderer> renderer) : Scene(renderer) { }

	/// Virtual destructor
	virtual ~ScreenElementScene()
	{
		CB_WARNING("~ScreenElementScene()");
	}

	/// Recreates anything that might have been lost while the game is running (because of system sleep, etc). Also used for initialization
	virtual HRESULT OnRestore() { Scene::OnRestore(); return S_OK; }

	/// Render the scene
	virtual HRESULT OnRender(float time, float deltaTime) { Scene::OnRender(); return S_OK; }

	/// Method to handle the device being lost
	virtual HRESULT OnLostDevice() { Scene::OnLostDevice(); return S_OK; }

	/// Return the Z-order value of the element
	virtual int GetZOrder() const { return 0; }

	/// Set the Z-order value of the element
	virtual void SetZOrder(const int zOrder) { }

	/// Return whether or not the element is visible
	virtual bool IsVisible() const { return true; }

	/// Set an element visible or invisible
	virtual void SetVisible(bool visible) { }

	/// Msg callback from the view to the screen element
	virtual LRESULT CALLBACK OnMsgProc(AppMsg msg) { return 0; }

	/// Update method for a screen element
	virtual void OnUpdate(float deltaTime) { Scene::OnUpdate(deltaTime); }

	/// Add a child element to the scene graph
	virtual bool AddChild(GameObjectId id, shared_ptr<ISceneNode> child) { return Scene::AddChild(id, child); }
};
