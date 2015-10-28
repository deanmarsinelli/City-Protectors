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
	virtual HRESULT OnRestore() override;
	virtual HRESULT OnRender(float time, float deltaTime) override;

	/// Return 1 since this is in the front of the screen
	virtual int GetZOrder() const override { return 1; }

	/// Disable setting the Z order
	virtual void SetZOrder(const int zOrder) override { }

	virtual LRESULT CALLBACK OnMsgProc(AppMsg msg) override;

	static void CALLBACK OnGuiEvent(UINT nEvents, int nControlID, CDXUTControl* pControl, void* pUserContext);

protected:
	/// Dialog for standard controls
	CDXUTDialog m_HUD;
};
