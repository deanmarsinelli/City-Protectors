/*
	MainMenuUI.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <DXUT.h>
#include <DXUTGui.h>
#include <UserInterface.h>

class MainMenuUI : public BaseUI
{
public:
	MainMenuUI();
	virtual ~MainMenuUI();

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
	void CALLBACK _OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void *pUserContext);

	void Set();

protected:
	/// Dialog for sample specific controls
	CDXUTDialog m_SampleUI;

	bool m_CreatingGame;

	int m_NumAIs;

	int m_NumPlayers;

	std::vector<std::wstring> m_Levels;

	int m_LevelIndex;

	std::string m_HostName;

	std::string m_HostListenPort;

	std::string m_ClientAttachPort;
};
