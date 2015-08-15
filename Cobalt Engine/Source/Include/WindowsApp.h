/*
	WindowsApp.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <DXUT.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <windows.h>

#include "BaseGameLogic.h"
#include "EngineStd.h"
#include "HumanView.h"
#include "Initialization.h"
#include "types.h"

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

	HumanView* GetHumanView();

	/// Manages displaying a modal UI box on the screen
	int Modal(shared_ptr<IScreenElement> pModalScreen, int defaultAnswer);

	// Renderer
	enum Renderer
	{
		Renderer_Unknown,
		Renderer_D3D9,
		Renderer_D3D11
	};

	static Renderer GetRendererImpl();

	// General Callbacks
	static void CALLBACK OnUpdate(double time, float deltaTime, void* pUserContext);
	static bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext);

	// D3D9 Callbacks
	static bool CALLBACK IsD3D9DeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext);
	static HRESULT CALLBACK OnD3D9CreateDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	static HRESULT CALLBACK OnD3D9ResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	static void CALLBACK OnD3D9FrameRender(IDirect3DDevice9* pd3dDevice, double time, float deltaTime, void* pUserContext);
	static void CALLBACK OnD3D9LostDevice(void* pUserContext);
	static void CALLBACK OnD3D9DestroyDevice(void* pUserContext);

	// D3D11 Callbacks
	static bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext);
	static HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	static HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	static void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext);
	static void CALLBACK OnD3D11DestroyDevice(void* pUserContext);
	static void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double time, float deltaTime, void* pUserContext);

	virtual BaseGameLogic* CreateGameAndView() = 0;

	int GetExitCode() { return DXUTGetExitCode(); }
protected:
	virtual void RegisterGameEvents() { }
	virtual void CreateNetworkEventForwarder();
	virtual void DestroyNetworkEventForwarder();

	/// Process messages in this pump until a specific message is received
	int PumpUntilMessage(UINT msgEnd, WPARAM* pWParam, LPARAM* pLParam);
	void FlashWhileMinimized();

private:
	void RegisterEngineEvents();

	// Member variables
public:
	// Game Properties
	BaseGameLogic* m_pGame;
	GameOptions m_Options;
	std::shared_ptr<IRenderer> m_Renderer;
	class ResCache* m_ResCache;

protected:
	HINSTANCE m_hInstance;
	bool m_WindowedMode;
	bool m_IsRunning;
	bool m_QuitRequested;
	bool m_Quitting;
	Rect m_RCDesktop;
	Point m_ScreenSize;
	int m_ColorDepth;
	bool m_IsEditorRunning;

	std::unordered_map<std::wstring, std::wstring> m_TextResource;
	std::unordered_map<std::wstring, unsigned int> m_HotKeys;
	
	/// A bit field used to keep track of how many modal dialog's are displayed
	int m_HasModalDialog;
};

extern WindowsApp* g_pApp;