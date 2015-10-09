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

#include "BaseSocketManager.h"
#include "EventManager.h"
#include "Initialization.h"
#include "NetworkEventForwarder.h"
#include "types.h"

class BaseGameLogic;
class IRenderer;
class IScreenElement;
class HumanView;

/**
	Application Layer interface for a Windows Game.
*/
class WindowsApp
{
public:
	/// Default constructor
	WindowsApp();

	// Windows Application Data -- Must define these in an inherited class for the game
	/// Return the title of the game
	virtual TCHAR* GetGameTitle() = 0;
	/// Return the application directory for saved files
	virtual TCHAR* GetGameAppDirectory() = 0;
	/// Return the icon for the game
	virtual HICON GetIcon() = 0;

	// Win32 Stuff
	/// Return the handle to the window
	HWND GetHwnd();
	
	/// Return the hInstance of the application
	HINSTANCE GetInstance();
	
	/// Initialize the application layer
	virtual bool InitInstance(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd = nullptr, int screenWidth = 800, int screenHeight = 600);
	
	/// Message procedure callback for handling messages from the operating system
	static LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pDoneProcessing, void* pUserContext);
	
	/// Handler for the WM_DISPLAYCHANGE message
	LRESULT OnDisplayChange(int colorDepth, int width, int height);
	
	/// Handler for the WM_POWERBROADCAST message
	LRESULT OnPowerBroadcast(int event);
	
	/// Handler for the WM_SYSCOMMAND message
	LRESULT OnSysCommand(WPARAM wParam, LPARAM lParam);
	
	/// Handles the WM_CLOSE message
	LRESULT OnClose();

	/// How to handle Alt+Enter 
	LRESULT OnAltEnter();

	/// Handles the WM_NCCREATE message
	LRESULT OnNcCreate(LPCREATESTRUCT cs);

	/// Create the intial game logic and view
	virtual BaseGameLogic* CreateGameAndView() = 0;

	/// Load a game
	virtual bool LoadGame();

	/// Abort the game
	void AbortGame();

	/// Return the exit code for the game
	int GetExitCode();

	/// Is the game currently running?
	bool IsRunning();

	/// Set whether the game should begin quitting or not
	void SetQuitting(bool quitting);

	/// Returns a pointer to the game logic
	BaseGameLogic* GetGameLogic() const;

	/// Load game strings from a resource file
	bool LoadStrings(std::string language);

	/// Return a string from a string resource ID
	std::wstring GetString(std::wstring sID);
	//int GetHotKeyForString(std::wstring sID);
	/// Map a char to a keycode
	UINT MapCharToKeycode(const char hotkey);

	/// Is a modal dialog on screen?
	bool HasModalDialog();

	/// Force exit out of a modal
	void ForceModalExit();

	/// Return a pointer to the human view for the application
	HumanView* GetHumanView();

	/// Return the screen size
	const Point& GetScreenSize();

	/// Manages displaying a modal UI box on the screen
	int Modal(std::shared_ptr<IScreenElement> pModalScreen, int defaultAnswer);

	/// What type of renderer is the game using
	enum Renderer
	{
		Renderer_Unknown,
		Renderer_D3D9,
		Renderer_D3D11
	};

	/// Return the type of render the game is using
	static Renderer GetRendererImpl();

	// General Callbacks
	/// Main update method for the application, called once per frame
	static void CALLBACK OnUpdate(double time, float deltaTime, void* pUserContext);
	/// Called right before creating a D3D9 or D3D11 device, allows the application to modify settings
	static bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext);

	// D3D9 Callbacks
	/// Reject any device that isn't acceptable by the application
	static bool CALLBACK IsD3D9DeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext);
	/// Create any D3D9 resources that will live through a device reset
	static HRESULT CALLBACK OnD3D9CreateDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	/// Create any D3D9 resources that won't live through a device reset
	static HRESULT CALLBACK OnD3D9ResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	/// Called at the end of every frame to perform all rendering calls for the scene
	static void CALLBACK OnD3D9FrameRender(IDirect3DDevice9* pd3dDevice, double time, float deltaTime, void* pUserContext);
	/// Release D3D9 resources created in the OnD3D9ResetDevice callback
	static void CALLBACK OnD3D9LostDevice(void* pUserContext);
	/// Release D3D9 resources created in the OnD3D9CreateDevice callback
	static void CALLBACK OnD3D9DestroyDevice(void* pUserContext);

	// D3D11 Callbacks
	/// Reject any device that isn't acceptable by the application
	static bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext);
	/// Create any D3D11 resources that aren't dependent on the back buffer
	static HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	/// Create any D3D11 resources that depend on the back buffer
	static HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	static void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double time, float deltaTime, void* pUserContext);
	/// Release D3D11 resources created in OnD3D11ResizedSwapChain 
	static void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext);
	/// Release D3D11 resources created in OnD3D11CreateDevice
	static void CALLBACK OnD3D11DestroyDevice(void* pUserContext);
	
	/// Attach this application layer as a client to a server
	bool AttachAsClient();

protected:
	/// Register game events (should be overridden)
	virtual void RegisterGameEvents() { }
	virtual void CreateNetworkEventForwarder();
	virtual void DestroyNetworkEventForwarder();

	/// Process messages in this pump until a specific message is received
	int PumpUntilMessage(UINT msgEnd, WPARAM* pWParam, LPARAM* pLParam);

	/// Flash the game icon while minimized
	void FlashWhileMinimized();

private:
	/// Register engine events
	void RegisterEngineEvents();

public:
	/// Pointer to the game logic layer
	BaseGameLogic* m_pGame;

	/// Socket manager for the application layer
	BaseSocketManager* m_pBaseSocketManager;

	/// Network event forwarder to send outgoing events over network
	NetworkEventForwarder* m_pNetworkEventForwarder;

	/// Options currently loaded for the game
	GameOptions m_Options;

	/// Pointer to the renderer
	std::shared_ptr<IRenderer> m_Renderer;

	/// Pointer to the resource cache for the game
	class ResCache* m_ResCache;

	/// Event manager for the game
	EventManager* m_pEventManager;

protected:
	/// Instance handle to the application
	HINSTANCE m_hInstance;

	/// Whether the game is running in windowed mode or full screen
	bool m_WindowedMode;

	/// If the game is currently running
	bool m_IsRunning;

	/// Has quit been requested by the user?
	bool m_QuitRequested;

	/// Is the game currently shutting down
	bool m_Quitting;
	Rect m_RCDesktop;

	/// Size of the display screen in pixels
	Point m_ScreenSize;
	int m_ColorDepth;

	/// Is the editor currently running
	bool m_IsEditorRunning;

	std::unordered_map<std::wstring, std::wstring> m_TextResource;
	std::unordered_map<std::wstring, unsigned int> m_HotKeys;
	
	/// A bit field used to keep track of how many modal dialog's are displayed
	int m_HasModalDialog;
};

/// Global pointer to the application layer
extern WindowsApp* g_pApp;
