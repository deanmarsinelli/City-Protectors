/*
	StandardHUD.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <DXUT.h>
#include <DXUTGui.h>
#include <UserInterface.h>

/**
	This is the standard HUD UI during the game.
*/
class StandardHUD : public BaseUI
{
public:
	/// Default constructor
	StandardHUD();

	/// Virtual destructor
	virtual ~StandardHUD();

	// IScreenElement interface implementation
	/// Called when the device is restored
	virtual HRESULT OnRestore() override;

	/// Render a frame
	virtual HRESULT OnRender(float time, float deltaTime) override;

	/// Return 1 since this is in the front of the screen
	virtual int GetZOrder() const override { return 1; }

	/// Disable setting the Z order
	virtual void SetZOrder(const int zOrder) override { }

	/// Handle messages from the OS
	virtual LRESULT CALLBACK OnMsgProc(AppMsg msg) override;

	/// Callback for DXUT GUI events
	static void CALLBACK OnGuiEvent(UINT nEvents, int nControlID, CDXUTControl* pControl, void* pUserContext);

protected:
	/// Dialog for standard controls
	CDXUTDialog m_HUD;
};
