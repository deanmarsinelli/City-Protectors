/*
	WindowsApp.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <DXUT.h>

#include "WindowsApp.h"

#include "BaseGameLogic.h"
#include "ClientSocketManager.h"
#include "D3DRenderer9.h"
#include "D3DRenderer11.h"
#include "EngineStd.h"
#include "EventManager.h"
#include "Events.h"
#include "D3DRenderer.h"
#include "Logger.h"
#include "LuaScriptExports.h"
#include "LuaScriptProcess.h"
#include "MessageBox.h"
#include "NetworkEvents.h"
#include "PhysicsEvents.h"
#include "Resource.h"
#include "ResourceZipFile.h"
#include "ScriptComponent.h"
#include "StringUtil.h"
#include "XmlResource.h"

// global pointer for the engine to the application instance
// this will get set in the constructor of the WindowsApp
// class, but will point to a derived instance that is
// global and instantiated before the application begins
WindowsApp* g_pApp = nullptr;

// pre-init lua script
const char* SCRIPT_PREINIT_FILE = "Scripts\\PreInit.lua";

//====================================================
//	WindowsApp
//	Public method definitions
//====================================================
WindowsApp::WindowsApp()
{
	g_pApp = this;
	m_pGame = nullptr;

	m_RCDesktop.top = m_RCDesktop.left = m_RCDesktop.bottom = m_RCDesktop.right = 0;
	m_ScreenSize = Point(0, 0);
	m_ColorDepth = 32;

	m_IsRunning = false;
	m_IsEditorRunning = false;
	
	m_HasModalDialog = 0;

	m_pEventManager = nullptr;
	m_ResCache = nullptr;

	m_pNetworkEventForwarder = nullptr;
	m_pBaseSocketManager = nullptr;

	m_QuitRequested = false;
	m_Quitting = false;

}

HWND WindowsApp::GetHwnd()
{
	return DXUTGetHWND();
}

HINSTANCE WindowsApp::GetInstance()
{
	return m_hInstance;
}

bool WindowsApp::InitInstance(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd, int screenWidth, int screenHeight)
{
	// check for an existing instance of the game
#ifndef _DEBUG
	if (!IsOnlyInstance(GetGameTitle()))
		return false;
#endif

	// turn off mouse cursor
	SetCursor(NULL);

	// check for system resources
	bool resourceCheck = false;
	while (!resourceCheck)
	{
		// un-hardcode these in the future if you want to use them appropriately
		const DWORDLONG physicalRAM = 512 * MEGABYTE;
		const DWORDLONG virtualRAM = 1024 * MEGABYTE;
		const DWORDLONG diskSpace = 10 * MEGABYTE;

		if (!CheckStorage(diskSpace))
			return false;

		if (!CheckMemory(physicalRAM, virtualRAM))
			return false;

		const DWORD minCpuSpeed = 1300;	// 1.3Ghz
		DWORD thisCPU = ReadCPUSpeed();
		if (thisCPU < minCpuSpeed)
		{
			CB_ERROR("GetCPUSpeed reports CPU is too slow for this game.");
			return false;
		}

		resourceCheck = true;
	}

	m_hInstance = hInstance;

	// register events
	RegisterEngineEvents();
	RegisterGameEvents();

	// initialize resource cache
	IResourceFile* zipFile = (m_IsEditorRunning || m_Options.m_UseDevelopmentDirectories) ?
		CB_NEW DevelopmentResourceZipFile(L"Assets.zip", DevelopmentResourceZipFile::Editor) :
		CB_NEW ResourceZipFile(L"Assets.zip");

	m_ResCache = CB_NEW ResCache(50, zipFile);
	if (!m_ResCache->Init())
	{
		CB_ERROR("Failed to initialize resource cache. Check paths");
		return false;
	}
	// register resource loaders
	extern shared_ptr<IResourceLoader> CreateWAVResourceLoader();
	extern shared_ptr<IResourceLoader> CreateDDSResourceLoader();
	extern shared_ptr<IResourceLoader> CreateJPGResourceLoader();
	extern shared_ptr<IResourceLoader> CreateXmlResourceLoader();
	extern shared_ptr<IResourceLoader> CreateSdkMeshResourceLoader();
	extern shared_ptr<IResourceLoader> CreateLuaScriptResourceLoader();
	m_ResCache->RegisterLoader(CreateWAVResourceLoader());
	m_ResCache->RegisterLoader(CreateDDSResourceLoader());
	m_ResCache->RegisterLoader(CreateJPGResourceLoader());
	m_ResCache->RegisterLoader(CreateXmlResourceLoader());
	m_ResCache->RegisterLoader(CreateSdkMeshResourceLoader());
	m_ResCache->RegisterLoader(CreateLuaScriptResourceLoader());

	if (!LoadStrings("English"))
	{
		CB_ERROR("Failed to load strings");
		return false;
	}

	// create the lua state manager and run the initial script
	if (!LuaStateManager::Create())
	{
		CB_ERROR("Failed to initialize Lua");
		return false;
	}
	// this is scoped so it will load into the cache then destroy the local resource
	{
		Resource res(SCRIPT_PREINIT_FILE);
		shared_ptr<ResHandle> pResourceHandle = m_ResCache->GetHandle(&res);
	}
	// register functions exported to lua FROM C++
	LuaScriptExports::Register();
	LuaScriptProcess::RegisterScriptClass();
	LuaScriptComponent::RegisterScriptFunctions();


	// create event manager
	m_pEventManager = CB_NEW EventManager("Event Manager", true);
	if (!m_pEventManager)
	{
		CB_ERROR("Failed to create event manager");
		return false;
	}
	

	// DirectX initialization
	DXUTInit(true, true, lpCmdLine, true);
	if (hWnd == nullptr)
	{
		DXUTCreateWindow(GetGameTitle(), hInstance, GetIcon());
	}
	else
	{
		DXUTSetWindow(hWnd, hWnd, hWnd);
	}

	if (!GetHwnd())
	{
		CB_ERROR("Failed to get HWND");
		return false;
	}
	SetWindowText(GetHwnd(), GetGameTitle());

	// initialize directory to store save game files
	wcscpy_s(m_SaveGameDirectory, GetSaveGameDirectory(GetHwnd(), GetGameAppDirectory()));

	m_ScreenSize = Point(screenWidth, screenHeight);

	// create the d3d device
	//DXUTCreateDevice(D3D_FEATURE_LEVEL_9_3, true, screenWidth, screenHeight);
	DXUTCreateDevice(D3D_FEATURE_LEVEL_10_1, true, screenWidth, screenHeight);

	if (GetRendererImpl() == Renderer_D3D9)
	{
		m_Renderer = std::shared_ptr<IRenderer>(new D3DRenderer9());
	}
	else if (GetRendererImpl() == Renderer_D3D11)
	{
		m_Renderer = std::shared_ptr<IRenderer>(new D3DRenderer11());
	}
	m_Renderer->SetBackgroundColor(255, 20, 20, 200);
	m_Renderer->OnRestore();
	 
	// create the game logic
	m_pGame = CreateGameAndView();
	if (!m_pGame)
		return false;
	
	// preload resources
	m_ResCache->PreLoad("*.dds", nullptr);
	m_ResCache->PreLoad("*.jpg", nullptr);
	m_ResCache->PreLoad("*.wav", nullptr);
	if (GetRendererImpl() == WindowsApp::Renderer_D3D11)
	{
		m_ResCache->PreLoad("*.sdkmesh", nullptr);
	}

	m_IsRunning = true;

	return true;
}

LRESULT CALLBACK WindowsApp::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pDoneProcessing, void* pUserContext)
{
	LRESULT result = 0;
	switch (uMsg)
	{
	case WM_CLOSE:
		result = g_pApp->OnClose();
		break;

	// foward these windows messages to the game views
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case MM_JOY1BUTTONDOWN:
	case MM_JOY1BUTTONUP:
	case MM_JOY1MOVE:
	case MM_JOY1ZMOVE:
	case MM_JOY2BUTTONDOWN:
	case MM_JOY2BUTTONUP:
	case MM_JOY2MOVE:
	case MM_JOY2ZMOVE:
	{
		if (g_pApp->m_pGame)
		{
			GameViewList viewList = g_pApp->m_pGame->GetGameViewList();
			AppMsg msg;
			msg.m_hWnd = hWnd;
			msg.m_uMsg = uMsg;
			msg.m_wParam = wParam;
			msg.m_lParam = lParam;
			// iterate the view list in reverse, forward the message to the view that is on top (last in the list)
			for (GameViewList::reverse_iterator it = viewList.rbegin(); it != viewList.rend(); ++it)
			{
				// if the view returns true, it has consumed this message
				if ((*it)->OnMsgProc(msg))
				{
					result = true;
					break;
				}
			}
		}
		break;
	}
	}
	return result;
}

LRESULT WindowsApp::OnDisplayChange(int colorDepth, int width, int height)
{
	m_RCDesktop.left = 0;
	m_RCDesktop.top = 0;
	m_RCDesktop.right = width;
	m_RCDesktop.bottom = height;
	m_ColorDepth = colorDepth;

	return 0;
}

LRESULT WindowsApp::OnPowerBroadcast(int event)
{
	if (event == PBT_APMQUERYSUSPEND)
	{
		return BROADCAST_QUERY_DENY;
	}
	else if (event == PBT_APMBATTERYLOW)
	{
		AbortGame();
	}

	return true;
}

LRESULT WindowsApp::OnSysCommand(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case SC_MAXIMIZE:
	{
		if (m_WindowedMode && m_IsRunning)
		{
			// Maximize into full screen
			OnAltEnter();
		}
		return 0;
	}
	case SC_CLOSE:
	{
		if (lParam != g_QuitNoPrompt)
		{
			if (m_QuitRequested)
				return true;

			m_QuitRequested = true;

			if (CBMessageBox::Ask(QUESTION_QUIT_GAME) == IDNO)
			{
				m_QuitRequested = false;

				return true;
			}

			m_Quitting = true;

			if (HasModalDialog())
			{
				ForceModalExit();

				PostMessage(GetHwnd(), WM_SYSCOMMAND, SC_CLOSE, g_QuitNoPrompt);

				m_QuitRequested = false;
				return true;
			}
			m_QuitRequested = false;

		}
		return 0;
	}
	default:
		return DefWindowProc(GetHwnd(), WM_SYSCOMMAND, wParam, lParam);
	}

	return 0;
}

LRESULT WindowsApp::OnClose()
{
	// release resources in reverse order

	CB_SAFE_DELETE(m_pGame);
	DestroyWindow(GetHwnd());
	DestroyNetworkEventForwarder();

	CB_SAFE_DELETE(m_pBaseSocketManager);
	CB_SAFE_DELETE(m_pEventManager);

	LuaScriptComponent::UnregisterScriptFunctions();
	LuaScriptExports::Unregister();
	LuaStateManager::Destroy();

	CB_SAFE_DELETE(m_ResCache);

	return 0;
}

LRESULT WindowsApp::OnAltEnter()
{
	return 0;
}

LRESULT WindowsApp::OnNcCreate(LPCREATESTRUCT cs)
{
	return 0;
}

bool WindowsApp::LoadGame()
{
	return m_pGame->LoadGame(m_Options.m_Level.c_str());
}

void WindowsApp::AbortGame()
{
	m_Quitting = true;
}

int WindowsApp::GetExitCode()
{
	return DXUTGetExitCode();
}

bool WindowsApp::IsRunning()
{
	return m_IsRunning;
}

void WindowsApp::SetQuitting(bool quitting)
{
	m_Quitting = quitting;
}

BaseGameLogic* WindowsApp::GetGameLogic() const
{
	return m_pGame;
}

bool WindowsApp::LoadStrings(std::string language)
{
	// get the correct xml file
	std::string languageFile = "Strings\\";
	languageFile += language;
	languageFile += ".xml";

	// load the file from resource cache
	TiXmlElement* pRoot = XmlResourceLoader::LoadAndReturnRootXmlElement(languageFile.c_str());
	if (!pRoot)
	{
		CB_ERROR("String file is missing");
		return false;
	}

	// loop through each element and load the string/id pairs
	for (auto pElem = pRoot->FirstChildElement(); pElem; pElem = pElem->NextSiblingElement())
	{
		const char* pKey = pElem->Attribute("id");
		const char* pValue = pElem->Attribute("value");
		const char* pHotkey = pElem->Attribute("hotkey");

		if (pKey && pValue)
		{
			wchar_t wideKey[64];
			wchar_t wideText[1024];
			
			// convert to wide characters
			AnsiToWideCch(wideKey, pKey, 64);
			AnsiToWideCch(wideText, pValue, 1024);

			// insert into the string map
			m_TextResource[std::wstring(wideKey)] = std::wstring(wideText);

			if (pHotkey)
			{
				m_HotKeys[std::wstring(wideKey)] = MapCharToKeycode(*pHotkey);
			}
		}
	}

	return true;
}

std::wstring WindowsApp::GetString(std::wstring sID)
{
	// return the localized string from the string cache
	auto localizedString = m_TextResource.find(sID);
	if (localizedString == m_TextResource.end())
	{
		CB_ASSERT(0 && "String not found");
		return L"";
	}
	return localizedString->second;
}

UINT WindowsApp::MapCharToKeycode(const char hotkey)
{
	if (hotkey >= '0' && hotkey <= '9')
	{
		return 0x30 + hotkey - '0';
	}
	if (hotkey >= 'A' && hotkey <= 'Z')
	{
		return 0x41 + hotkey - 'A';
	}

	CB_ASSERT(0 && "Platform specfic hotkey is undefined");
	return 0;
}

bool WindowsApp::HasModalDialog()
{
	return m_HasModalDialog != 0;
}

void WindowsApp::ForceModalExit()
{
	PostMessage(GetHwnd(), g_MsgEndModal, 0, g_QuitNoPrompt);
}

HumanView* WindowsApp::GetHumanView()
{
	HumanView* pView = nullptr;
	GameViewList views = m_pGame->GetGameViewList();

	// iterate the list of views looking for a human view
	for (GameViewList::iterator it = views.begin(); it != views.end(); ++it)
	{
		if ((*it)->GetType() == GameViewType::GameView_Human)
		{
			shared_ptr<IGameView> pIGameView(*it);
			pView = static_cast<HumanView *>(&*pIGameView);
			break;
		}
	}

	return pView;
}

const Point& WindowsApp::GetScreenSize()
{
	return m_ScreenSize;
}

int WindowsApp::Modal(shared_ptr<IScreenElement> pModalScreen, int defaultAnswer)
{
	// find a human view to display the dialog box
	HumanView* pView = GetHumanView();
	if (!pView)
	{
		// no human view to use
		return defaultAnswer;
	}

	CB_ASSERT(GetHwnd() != nullptr && L"Main Window is NULL");

	if (m_HasModalDialog & 0x10000000)
	{
		CB_ASSERT(0 && "Too many nested dialogs!");
		return defaultAnswer;
	}

	if (GetHwnd() != nullptr && IsIconic(GetHwnd()))
	{
		FlashWhileMinimized();
	}

	// each new modal shifts the bits left by 1
	m_HasModalDialog <<= 1;
	m_HasModalDialog |= 1;

	// push the modal screen onto the view
	pView->PushElement(pModalScreen);

	// process messages in the background while the modal is up using this surrogate windows message pump
	LPARAM lParam = 0;
	int result = PumpUntilMessage(g_MsgEndModal, NULL, &lParam);

	if (lParam != 0)
	{
		if (lParam == g_QuitNoPrompt)
		{
			result = defaultAnswer;
		}
		else
		{
			result = (int)lParam;
		}
	}
	// remove the modal
	pView->RemoveElement(pModalScreen);
	m_HasModalDialog >>= 1;
	
	return result;
}

WindowsApp::Renderer WindowsApp::GetRendererImpl()
{
	DXUTDeviceVersion version = DXUTGetDeviceSettings().ver;
	if (version == DXUT_D3D11_DEVICE)
	{
		return Renderer::Renderer_D3D11;
	}
	else if (version == DXUT_D3D9_DEVICE)
	{
		return Renderer::Renderer_D3D9;
	}

	return Renderer::Renderer_Unknown;
}

// CALLBACKS
void CALLBACK WindowsApp::OnUpdate(double time, float deltaTime, void* pUserContext)
{
	// dont update the scene if there is a modal up
	if (g_pApp->HasModalDialog())
	{
		return;
	}

	// if the game is quitting, post the close message
	if (g_pApp->m_Quitting)
	{
		PostMessage(g_pApp->GetHwnd(), WM_CLOSE, 0, 0);
	}

	// otherwise, process events and update the current game logic
	if (g_pApp->m_pGame)
	{
		// allow 10 ms of events to process
		IEventManager::Get()->Update(10);

		g_pApp->m_pGame->OnUpdate((float)time, deltaTime);
	}

}

bool CALLBACK WindowsApp::ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
	// if the system is creating a d3d9 device, adjust some settings
	if (pDeviceSettings->ver == DXUT_D3D9_DEVICE)
	{
		IDirect3D9* pD3D = DXUTGetD3D9Object();
		D3DCAPS9 caps;
		pD3D->GetDeviceCaps(pDeviceSettings->d3d9.AdapterOrdinal, pDeviceSettings->d3d9.DeviceType, &caps);

		// if device doesnt support HW transform and light or at least 1.1 vertex shaders in hardware then use software vertex processing
		if ((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 || caps.VertexShaderVersion < D3DVS_VERSION(1, 1))
		{
			pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}
	}
	// for the device created, if its a reference device, display a warning box
	static bool s_FirstTime = true;
	if (s_FirstTime)
	{
		if ((pDeviceSettings->ver == DXUT_D3D9_DEVICE && pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF) ||
			(pDeviceSettings->ver == DXUT_D3D11_DEVICE && pDeviceSettings->d3d11.DriverType == D3D_DRIVER_TYPE_REFERENCE))
		{
			DXUTDisplaySwitchingToREFWarning(pDeviceSettings->ver);
		}
	}

	return true;
}

// D3D9 Callbacks
bool CALLBACK WindowsApp::IsD3D9DeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
{
	// skip any backbuffer formats that don't support alpha blending
	IDirect3D9* pD3D = DXUTGetD3D9Object();
	if (FAILED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType, AdapterFormat,
		D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, D3DRTYPE_TEXTURE, BackBufferFormat)))
	{
		return false;
	}
	
	// reject any device that doesn't support at least pixel shader 2.0
	if (pCaps->PixelShaderVersion < D3DPS_VERSION(2, 0))
	{
		return false;
	}

	return true;
}

HRESULT CALLBACK WindowsApp::OnD3D9CreateDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	HRESULT hr;

	V_RETURN(D3DRenderer::g_DialogResourceManager.OnD3D9CreateDevice(pd3dDevice));

	return S_OK;
}

HRESULT CALLBACK WindowsApp::OnD3D9ResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	HRESULT hr;

	if (g_pApp->m_Renderer)
	{
		// restore the renderer
		V_RETURN(g_pApp->m_Renderer->OnRestore());
	}
	
	if (g_pApp->m_pGame)
	{
		// call OnRestore() on all game views
		GameViewList views = g_pApp->m_pGame->GetGameViewList();
		for (GameViewList::iterator it = views.begin(); it != views.end(); ++it)
		{
			V_RETURN((*it)->OnRestore());
		}
	}

	return S_OK;
}

void CALLBACK WindowsApp::OnD3D9FrameRender(IDirect3DDevice9* pd3dDevice, double time, float deltaTime, void* pUserContext)
{
	// iterate the views and call render on each one
	GameViewList views = g_pApp->m_pGame->GetGameViewList();
	for (GameViewList::iterator it = views.begin(); it != views.end(); ++it)
	{
		(*it)->OnRender((float)time, deltaTime);
	}

	g_pApp->m_pGame->RenderDiagnostics();
}

void CALLBACK WindowsApp::OnD3D9LostDevice(void* pUserContext)
{
	D3DRenderer::g_DialogResourceManager.OnD3D9LostDevice();
	
	if (g_pApp->m_pGame)
	{
		// call OnLostDevice() on all game views
		GameViewList views = g_pApp->m_pGame->GetGameViewList();
		for (GameViewList::iterator it = views.begin(); it != views.end(); ++it)
		{
			(*it)->OnLostDevice();
		}
	}
}

void CALLBACK WindowsApp::OnD3D9DestroyDevice(void* pUserContext)
{
	// shut down and destroy the renderer
	if (g_pApp->m_Renderer)
	{
		g_pApp->m_Renderer->Shutdown();
	}
	D3DRenderer::g_DialogResourceManager.OnD3D9DestroyDevice();
	g_pApp->m_Renderer = nullptr;
}

// D3D11 Callbacks
bool CALLBACK WindowsApp::IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
{
	// accept all d3d11 devices
	return true;
}

HRESULT CALLBACK WindowsApp::OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	HRESULT hr;
	
	ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();
	V_RETURN(D3DRenderer::g_DialogResourceManager.OnD3D11CreateDevice(pd3dDevice, pd3dImmediateContext));

	return S_OK;
}

HRESULT CALLBACK WindowsApp::OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	HRESULT hr;

	V_RETURN(D3DRenderer::g_DialogResourceManager.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));

	if (g_pApp->m_pGame)
	{
		// call OnRestore() on all game views
		GameViewList views = g_pApp->m_pGame->GetGameViewList();
		for (GameViewList::iterator it = views.begin(); it != views.end(); ++it)
		{
			(*it)->OnRestore();
		}
	}

	return S_OK;
}

void CALLBACK WindowsApp::OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double time, float deltaTime, void* pUserContext)
{
	// iterate the views and call render on each one
	GameViewList views = g_pApp->m_pGame->GetGameViewList();
	for (GameViewList::iterator it = views.begin(); it != views.end(); ++it)
	{
		(*it)->OnRender((float)time, deltaTime);
	}

	g_pApp->m_pGame->RenderDiagnostics();
}

void CALLBACK WindowsApp::OnD3D11ReleasingSwapChain(void* pUserContext)
{
	D3DRenderer::g_DialogResourceManager.OnD3D11ReleasingSwapChain();
}

void CALLBACK WindowsApp::OnD3D11DestroyDevice(void* pUserContext)
{
	// shut down and destroy the renderer
	if (g_pApp->m_Renderer)
	{
		g_pApp->m_Renderer->Shutdown();
	}
	D3DRenderer::g_DialogResourceManager.OnD3D11DestroyDevice();
	g_pApp->m_Renderer = nullptr;
}

bool WindowsApp::AttachAsClient()
{
	ClientSocketManager* pClient = CB_NEW ClientSocketManager(g_pApp->m_Options.m_GameHost, g_pApp->m_Options.m_ListenPort);

	// connect to a remote server
	if (!pClient->Connect())
	{
		return false;
	}

	g_pApp->m_pBaseSocketManager = pClient;
	CreateNetworkEventForwarder();

	return true;
}


//====================================================
//	WindowsApp
//	Protected method definitions
//====================================================
void WindowsApp::CreateNetworkEventForwarder()
{
	if (m_pNetworkEventForwarder != nullptr)
	{
		CB_ERROR("Overwriting the network event forwarder");
		CB_SAFE_DELETE(m_pNetworkEventForwarder);
	}

	m_pNetworkEventForwarder = CB_NEW NetworkEventForwarder(0);

	// set up network event forwarding for certain events
	IEventManager* pEventManger = IEventManager::Get();
	pEventManger->AddListener(fastdelegate::MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_RequestNewGameObject::sk_EventType);
	pEventManger->AddListener(fastdelegate::MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_EnvironmentLoaded::sk_EventType);
	pEventManger->AddListener(fastdelegate::MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_PhysCollision::sk_EventType);
}

void WindowsApp::DestroyNetworkEventForwarder()
{
	if (m_pNetworkEventForwarder)
	{
		IEventManager* pEventManger = IEventManager::Get();
		pEventManger->RemoveListener(fastdelegate::MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_RequestNewGameObject::sk_EventType);
		pEventManger->RemoveListener(fastdelegate::MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_EnvironmentLoaded::sk_EventType);
		pEventManger->RemoveListener(fastdelegate::MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), Event_PhysCollision::sk_EventType);
		CB_SAFE_DELETE(m_pNetworkEventForwarder);
	}
}

int WindowsApp::PumpUntilMessage(UINT msgEnd, WPARAM* pWParam, LPARAM* pLParam)
{
	int currentTime = timeGetTime();
	MSG msg;
	for (;;)
	{
		// check to see if there is a message in our queue
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (msg.message == WM_CLOSE)
			{
				m_Quitting = true;
				GetMessage(&msg, NULL, 0, 0);
				break;
			}
			else
			{
				if (GetMessage(&msg, NULL, NULL, NULL))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				// if the message is the end message signal, we're done
				if (msg.message == msgEnd)
				{
					break;
				}
			}
		}
		else
		{
			// if no message, update the game views but nothing else
			// there is a modal screen up, so the game is paused
			if (m_pGame)
			{
				int timeNow = timeGetTime();
				float deltaTime = (float)(timeNow - currentTime) * 1000.0f;
				GameViewList views = m_pGame->GetGameViewList();
				for (GameViewList::iterator it = views.begin(); it != views.end(); ++it)
				{
					(*it)->OnUpdate(deltaTime);
				}
				currentTime = timeNow;
				DXUTRender3DEnvironment();
			}
		}
	}
	if (pLParam)
	{
		*pLParam = msg.lParam;
	}
	if (pWParam)
	{
		*pWParam = msg.wParam;
	}

	return 0;
}

void WindowsApp::FlashWhileMinimized()
{
	// Flash the application on the taskbar
	// until it's restored.
	if (!GetHwnd())
	return;

	// Blink the application if we are minimized,
	// waiting until we are no longer minimized
	if (IsIconic(GetHwnd()))
	{
		// Make sure the app is up when creating a new screen
		// this should be the case most of the time, but when
		// we close the app down, minimized, and a confirmation
		// dialog appears, we need to restore
		DWORD now = timeGetTime();
		DWORD then = now;
		MSG msg;

		FlashWindow(GetHwnd(), true);

		while (true)
		{
			if (PeekMessage(&msg, NULL, 0, 0, 0))
			{
				if (msg.message != WM_SYSCOMMAND || msg.wParam != SC_CLOSE)
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				// Are we done?
				if (!IsIconic(GetHwnd()))
				{
					FlashWindow(GetHwnd(), false);
					break;
				}
			}
			else
			{
				now = timeGetTime();
				DWORD timeSpan = now > then ? (now - then) : (then - now);
				if (timeSpan > 1000)
				{
					then = now;
					FlashWindow(GetHwnd(), true);
				}
			}
		}
	}
}

//====================================================
//	WindowsApp
//	Private method definitions
//====================================================
void WindowsApp::RegisterEngineEvents()
{
	REGISTER_EVENT(Event_EnvironmentLoaded);
	REGISTER_EVENT(Event_RequestStartGame);
	REGISTER_EVENT(Event_PlaySound);

	// game objects
	REGISTER_EVENT(Event_NewGameObject);
	REGISTER_EVENT(Event_DestroyGameObject);
	REGISTER_EVENT(Event_MoveGameObject);
	REGISTER_EVENT(Event_RequestNewGameObject);
	REGISTER_EVENT(Event_RequestDestroyGameObject);

	// render components
	REGISTER_EVENT(Event_NewRenderComponent);
	REGISTER_EVENT(Event_ModifiedRenderComponent);

	// network events
	REGISTER_EVENT(Event_NetworkPlayerObjectAssignment);
	REGISTER_EVENT(Event_RemoteClient);
	REGISTER_EVENT(Event_RemoteEnvironmentLoaded);
}
