/**
	WindowsApp.h

	The WindowsApp class provides the application
	layer for a windows based game.

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <memory>
#include <string>
#include <windows.h>
#include "EngineStd.h"
#include "Initialization.h"

class IRenderer;

/**
	Application Layer interface for a Windows Game.
*/
class WindowsApp
{
public:
	WindowsApp();

	// Windows Application Data
	// Must define these in an inherited class for the game
	virtual TCHAR* GetGameTitle() = 0;
	virtual TCHAR* GetGameAppDirectory() = 0;
	virtual HICON GetIcon() = 0;

	// Win32 Stuff
	HWND GetHwnd();
	HINSTANCE GetInstance();
	virtual bool InitInstance(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd = nullptr, int screenWidth = SCREEN_WIDTH, int screenHeight = SCREEN_HEIGHT);
	static LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pDoneProcessing, void* pUserContext);
	bool HasModalDialog();
	void ForceModalExit();
	LRESULT OnDisplayChange(int colorDepth, int width, int height);
	LRESULT OnPowerBroadcast(int event);
	LRESULT OnSysCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnClose();

	// Game Application stuff
	LRESULT OnAltEnter();
	LRESULT OnNcCreate(LPCREATESTRUCT cs);

	bool LoadStrings(std::string language);
	std::wstring GetString(std::wstring sID);
	int GetHotKeyForString(std::wstring sID);
	UINT MapCharToKeycode(const char hotkey);

	int Modal();

	// Renderer
	enum Renderer
	{
		Renderer_Unknown,
		Renderer_D3D9,
		Renderer_D3D11
	};

	static Renderer GetRendererImpl();

protected:
	virtual void RegisterGameEvents() { }
	virtual void CreateNetworkEventForwarder();
	virtual void DestroyNetworkEventForwarder();

private:
	void RegisterEngineEvents();

	// Member variables
public:
	// Game Properties
	//BaseGameLogic *m_pGame;
	GameOptions m_Options;
	std::shared_ptr<IRenderer> m_Renderer;

protected:
	HINSTANCE m_hInstance;
	bool m_WindowedMode;
	bool m_IsRunning;
	bool m_QuitRequested;
	bool m_Quitting;
	// rect m_rcDesktop
	Point m_ScreenSize;
	int m_ColorDepth;
	bool m_IsEditorRunning;


};

extern WindowsApp* g_pApp;