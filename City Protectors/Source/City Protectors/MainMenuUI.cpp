/*
	MainMenuUI.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <BaseGameLogic.h>
#include <D3DRenderer.h>
#include <EngineStd.h>
#include <Events.h>
#include <LevelManager.h>
#include <Logger.h>
#include <StringUtil.h>

#include "MainMenuUI.h"

#define CID_CREATE_GAME_RADIO			(2)
#define CID_JOIN_GAME_RADIO				(3)
#define CID_NUM_AI_SLIDER				(4)
#define CID_NUM_PLAYER_SLIDER			(5)
#define CID_HOST_LISTEN_PORT			(6)
#define CID_CLIENT_ATTACH_PORT			(7)
#define CID_START_BUTTON				(8)
#define CID_HOST_NAME					(9)
#define CID_NUM_AI_LABEL				(10)
#define CID_NUM_PLAYER_LABEL			(11)
#define CID_HOST_LISTEN_PORT_LABEL		(12)
#define CID_CLIENT_ATTACH_PORT_LABEL	(13)
#define CID_HOST_NAME_LABEL				(14)
#define CID_LEVEL_LABEL					(15)
#define CID_LEVEL_LISTBOX				(16)

const int g_SampleUIWidth = 600;
const int g_SampleUIHeight = 600;

MainMenuUI::MainMenuUI()
{
	// initialize the DXUT UI and set the callback for GUI events
	m_SampleUI.Init(&D3DRenderer::g_DialogResourceManager);
	m_SampleUI.SetCallback(OnGuiEvent, this);
	
	int iY = 10;
	int iX = 35;
	int iX2 = g_SampleUIWidth / 2;
	int width = (g_SampleUIWidth / 2) - 10;
	int height = 25;
	int lineHeight = height + 2;

	// get defaults from game options
	m_NumAIs = g_pApp->m_Options.m_NumAIs;
	m_NumPlayers = g_pApp->m_Options.m_ExpectedPlayers;
	m_HostName = g_pApp->m_Options.m_GameHost;
	m_HostListenPort = g_pApp->m_Options.m_ListenPort;
	m_ClientAttachPort = g_pApp->m_Options.m_ListenPort;

	m_CreatingGame = true;

	// set color and font
	D3DCOLOR color = 0x50505050;
	m_SampleUI.SetBackgroundColors(color);

	m_SampleUI.SetFont(0, L"Ariel", height, 0);

	// add UI elements
	m_SampleUI.AddStatic(0, L"City Protectors Main Menu", iX - 20, iY, g_SampleUIWidth, height * 2);
	iY += (lineHeight * 3);

	m_SampleUI.AddRadioButton(CID_CREATE_GAME_RADIO, 1, L"Create Game", iX, iY, g_SampleUIWidth, height);
	iY += lineHeight;

	// add levels to the UI
	m_SampleUI.AddStatic(CID_LEVEL_LABEL, L"Level", iX, iY, width, height);
	m_SampleUI.AddListBox(CID_LEVEL_LISTBOX, iX2, iY, width, lineHeight * 5);
	std::vector<Level> levels = g_pApp->GetGameLogic()->GetLevelManager()->GetLevels;
	m_Levels.reserve(levels.size());
	int count = 0;
	for (auto it = levels.begin(); it != levels.end(); ++it, ++count)
	{
		m_Levels.push_back(s2ws(*it));
		m_SampleUI.GetListBox(CID_LEVEL_LISTBOX)->AddItem(m_Levels[count].c_str(), nullptr);
	}
	iY += (lineHeight * 5);

	// add num AIs slider
	m_SampleUI.AddStatic(CID_NUM_AI_LABEL, L"", iX, iY, width, height);
	m_SampleUI.AddSlider(CID_NUM_AI_SLIDER, iX2, iY, width, height);
	m_SampleUI.GetSlider(CID_NUM_AI_SLIDER)->SetRange(0, g_pApp->m_Options.m_MaxAIs);
	m_SampleUI.GetSlider(CID_NUM_AI_SLIDER)->SetValue(m_NumAIs); // default option
	iY += lineHeight;

	// add num players slider
	m_SampleUI.AddStatic(CID_NUM_PLAYER_LABEL, L"", iX, iY, width, height);
	m_SampleUI.AddSlider(CID_NUM_PLAYER_SLIDER, iX2, iY, width, height);
	m_SampleUI.GetSlider(CID_NUM_PLAYER_SLIDER)->SetRange(1, g_pApp->m_Options.m_MaxPlayers);
	m_SampleUI.GetSlider(CID_NUM_PLAYER_SLIDER)->SetValue(m_NumPlayers); // default option
	iY += lineHeight;

	// add networking input boxes
	m_SampleUI.AddStatic(CID_HOST_LISTEN_PORT_LABEL, L"Host Listen Port", iX, iY, width, height);
	m_SampleUI.AddEditBox(CID_HOST_LISTEN_PORT, L"57", iX2, iY, width, height * 2);
	CDXUTEditBox* box = m_SampleUI.GetEditBox(CID_HOST_LISTEN_PORT);
	box->SetVisible(false);
	iY += lineHeight * 3;

	m_SampleUI.AddRadioButton(CID_JOIN_GAME_RADIO, 1, L"Join Game", iX, iY, width, height);
	m_SampleUI.GetRadioButton(CID_JOIN_GAME_RADIO)->SetChecked(true);
	iY += lineHeight;

	m_SampleUI.AddStatic(CID_CLIENT_ATTACH_PORT_LABEL, L"Host Attach Port", iX, iY, width, height);
	m_SampleUI.AddEditBox(CID_CLIENT_ATTACH_PORT, L"57", iX2, iY, width, height * 2);
	iY += lineHeight * 3;

	m_SampleUI.AddStatic(CID_HOST_NAME_LABEL, L"Host Name", iX, iY, width, height);
	m_SampleUI.AddEditBox(CID_HOST_NAME, L"test name", iX2, iY, width, height * 2);
	iY += lineHeight;

	// start game button
	m_SampleUI.AddButton(CID_START_BUTTON, L"Start Game", (g_SampleUIWidth - (width / 2)) / 2, iY += lineHeight, width / 2, height);
	m_SampleUI.GetRadioButton(CID_CREATE_GAME_RADIO)->SetChecked(true);

	Set();
}

MainMenuUI::~MainMenuUI()
{
	D3DRenderer::g_DialogResourceManager.UnregisterDialog(&m_SampleUI);
}

HRESULT MainMenuUI::OnRestore()
{
	m_SampleUI.SetLocation((g_pApp->GetScreenSize().x - g_SampleUIWidth) / 2, (g_pApp->GetScreenSize().y - g_SampleUIHeight) / 2);
	m_SampleUI.SetSize(g_SampleUIWidth, g_SampleUIHeight);
	return S_OK;
}

HRESULT MainMenuUI::OnRender(float time, float deltaTime)
{
	HRESULT hr;

	// tell PIX what is happening
	DXUT_BeginPerfEvent(DXUT_PERFEVENTCOLOR, L"CityProtectorsHUD");
	V(m_SampleUI.OnRender(deltaTime));
	DXUT_EndPerfEvent();

	return S_OK;
}

LRESULT CALLBACK MainMenuUI::OnMsgProc(AppMsg msg)
{
	// forward the message to the DXUT dialog
	return m_SampleUI.MsgProc(msg.m_hWnd, msg.m_uMsg, msg.m_wParam, msg.m_lParam);
}

void CALLBACK MainMenuUI::OnGuiEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext)
{
	// forward this event on to the user context ui
	MainMenuUI* ui = static_cast<MainMenuUI*>(pUserContext);
	ui->_OnGUIEvent(nEvent, nControlID, pControl, pUserContext);
}

void CALLBACK MainMenuUI::_OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void *pUserContext)
{
	switch (nControlID)
	{
	case CID_CREATE_GAME_RADIO:
		m_CreatingGame = true;
		break;
	
	case CID_JOIN_GAME_RADIO:
		m_CreatingGame = false;
		break;
	
	case CID_LEVEL_LISTBOX:
	case CID_NUM_AI_SLIDER:
	case CID_NUM_PLAYER_SLIDER:
	case CID_HOST_LISTEN_PORT:
	case CID_CLIENT_ATTACH_PORT:
	case CID_HOST_NAME:
		break;
	

	case CID_START_BUTTON:
	{
		// start the game
		g_pApp->m_Options.m_NumAIs = m_NumAIs;
		g_pApp->m_Options.m_ExpectedPlayers = m_NumPlayers;
		if (m_CreatingGame)
		{
			if (m_LevelIndex == -1)
			{
				// FUTURE - error dialog would be good here
				return;
			}
			g_pApp->m_Options.m_Level = ws2s(m_Levels[m_LevelIndex]);
			g_pApp->m_Options.m_GameHost = "";
			g_pApp->m_Options.m_ListenPort = atoi(m_HostListenPort.c_str());
		}
		else
		{
			g_pApp->m_Options.m_GameHost = m_HostName;
			g_pApp->m_Options.m_ListenPort = atoi(m_ClientAttachPort.c_str());
		}

		SetVisible(false);

		// send an event to start the game
		shared_ptr<Event_RequestStartGame> pRequestStartGameEvent(CB_NEW Event_RequestStartGame());
		IEventManager::Get()->QueueEvent(pRequestStartGameEvent);
		break;
	}

	default:
	   CB_ERROR("Unknown control.");
	}

	// reset the UI
	Set();
}

void MainMenuUI::Set()
{
	WCHAR buffer[256];
	CHAR ansiBuffer[256];

	m_LevelIndex = m_SampleUI.GetListBox(CID_LEVEL_LISTBOX)->GetSelectedIndex();
	m_SampleUI.GetListBox(CID_LEVEL_LISTBOX)->SetVisible(m_CreatingGame);

	// number of AIs slider and text
	m_NumAIs = m_SampleUI.GetSlider(CID_NUM_AI_SLIDER)->GetValue();
	m_SampleUI.GetSlider(CID_NUM_AI_SLIDER)->SetVisible(m_CreatingGame);
	wsprintf(buffer, L"%s: %d\n", L"Number of AIs", m_NumAIs);
	m_SampleUI.GetStatic(CID_NUM_AI_LABEL)->SetText(buffer);
	m_SampleUI.GetStatic(CID_NUM_AI_LABEL)->SetVisible(m_CreatingGame);

	// number of players slider and text
	m_NumPlayers = m_SampleUI.GetSlider(CID_NUM_PLAYER_SLIDER)->GetValue();
	m_SampleUI.GetSlider(CID_NUM_PLAYER_SLIDER)->SetVisible(m_CreatingGame);
	wsprintf(buffer, L"%s: %d\n", L"Number of Players", m_NumPlayers);
	m_SampleUI.GetStatic(CID_NUM_PLAYER_LABEL)->SetText(buffer);
	m_SampleUI.GetStatic(CID_NUM_PLAYER_LABEL)->SetVisible(m_CreatingGame);

	// networking stuff
	m_SampleUI.GetStatic(CID_HOST_LISTEN_PORT_LABEL)->SetVisible((m_NumPlayers > 1) && m_CreatingGame);
	m_SampleUI.GetEditBox(CID_HOST_LISTEN_PORT)->SetVisible((m_NumPlayers > 1) && m_CreatingGame);
	if (m_CreatingGame)
	{
		WideToAnsiCch(ansiBuffer, m_SampleUI.GetEditBox(CID_HOST_LISTEN_PORT)->GetText(), 256);
		m_HostListenPort = ansiBuffer;
	}

	m_SampleUI.GetStatic(CID_HOST_NAME_LABEL)->SetVisible(!m_CreatingGame);
	m_SampleUI.GetEditBox(CID_HOST_NAME)->SetVisible(!m_CreatingGame);

	WideToAnsiCch(ansiBuffer, m_SampleUI.GetEditBox(CID_HOST_NAME)->GetText(), 256);
	m_HostName = ansiBuffer;

	m_SampleUI.GetStatic(CID_CLIENT_ATTACH_PORT_LABEL)->SetVisible(!m_CreatingGame);
	m_SampleUI.GetEditBox(CID_CLIENT_ATTACH_PORT)->SetVisible(!m_CreatingGame);

	if (!m_CreatingGame)
	{
		WideToAnsiCch(ansiBuffer, m_SampleUI.GetEditBox(CID_CLIENT_ATTACH_PORT)->GetText(), 256);
		m_ClientAttachPort = ansiBuffer;
	}
}
