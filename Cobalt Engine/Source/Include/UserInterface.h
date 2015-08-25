/*
	UserInterface.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "interfaces.h"
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

// TODO!
class ScreenElementScene : public IScreenElement, public Scene
{
public: 
	ScreenElementScene(shared_ptr<IRenderer> renderer) : Scene(renderer) { }


	/// Recreates anything that might have been lost while the game is running (because of system sleep, etc). Also used for initialization
	virtual HRESULT OnRestore();

	/// Render the screen element
	virtual HRESULT OnRender(float time, float deltaTime);

	/// Method to handle the device being lost
	virtual HRESULT OnLostDevice();

	/// Return the Z-order value of the element
	virtual int GetZOrder() const;

	/// Set the Z-order value of the element
	virtual void SetZOrder(const int zOrder);

	/// Return whether or not the element is visible
	virtual bool IsVisible() const;

	/// Set an element visible or invisible
	virtual void SetVisible(bool visible);

	/// Msg callback from the view to the screen element
	virtual LRESULT CALLBACK OnMsgProc(AppMsg msg);

	/// Update method for a screen element
	virtual void OnUpdate(float deltaTime);
};