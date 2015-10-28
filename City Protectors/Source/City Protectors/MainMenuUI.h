/*
	MainMenuUI.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <DXUT.h>
#include <DXUTGui.h>
#include <UserInterface.h>

/**
	This class is the UI for the main menu of the game.
*/
class MainMenuUI : public BaseUI
{
public:
	/// Default constructor sets up the main menu UI
	MainMenuUI();
	
	/// Virtual destructor destroys the UI elements
	virtual ~MainMenuUI();

	// IScreenElement interface implementation
	/// Called when the device is restored
	virtual HRESULT OnRestore() override;

	/// Render a frame
	virtual HRESULT OnRender(float time, float deltaTime) override;

	/// Return 1 since this is in the front of the screen
	virtual int GetZOrder() const override { return 1; }

	/// Disable setting the Z order
	virtual void SetZOrder(const int zOrder) override { }

	/// Handles messages from the OS
	virtual LRESULT CALLBACK OnMsgProc(AppMsg msg) override;

	/// Callback for DXUT GUI events
	static void CALLBACK OnGuiEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);

protected:
	/// Internal method that handle's GUI events sent from the callback
	void CALLBACK _OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void *pUserContext);

	/// Set up the GUI elements on screen
	void Set();

protected:
	/// Dialog for sample specific controls
	CDXUTDialog m_SampleUI;

	/// Is this player creating the game? or are they joining a game
	bool m_CreatingGame;

	/// Total number of AI's
	int m_NumAIs;

	/// Total number of players
	int m_NumPlayers;

	/// Name's of all the levels
	std::vector<std::wstring> m_Levels;

	int m_LevelIndex;

	/// Name of the host for a network game
	std::string m_HostName;

	/// Port that the host is listening on
	std::string m_HostListenPort;

	/// Port the clients should attach to
	std::string m_ClientAttachPort;
};
