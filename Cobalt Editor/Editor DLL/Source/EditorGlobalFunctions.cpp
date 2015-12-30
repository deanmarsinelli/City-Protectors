/*
	EditorGlobalFunctions.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham

	These functions will be exported through the DLL to be
	consumed by the C# editor application.
*/

#include <EngineStd.h>
#include <Events.h>
#include <GameObject.h>
#include <Logger.h>
#include <Raycast.h>
#include <StringUtil.h>

#include "EditorGlobalFunctions.h"

/*
	EditorMain

	This function is the entry point into the DLL and sets up the related objects. It does
	not have its own main loop, since the C# application will handle that.
*/
int EditorMain(int* instancePtrAddress, int* hPrevInstancePtrAddress, int* hWndPtrAddress, int nCmdShow, int screenWidth, int screenHeight)
{
	// C# passes in int pointers to the C++ DLL so we cast them as HINSTANCE's and HWND etc
	HINSTANCE hInstance = (HINSTANCE)instancePtrAddress;
	HINSTANCE hPrevInstance = (HINSTANCE)hPrevInstancePtrAddress;
	HWND hWnd = (HWND)hWndPtrAddress;
	WCHAR* lpCmdLine = L"";

	// set up memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Logger::Init("logging.xml");

	g_pApp->m_Options.Init("EditorOptions.xml", lpCmdLine);

	// set up DX callback functions
	DXUTSetCallbackMsgProc(WindowsApp::MsgProc);
	DXUTSetCallbackFrameMove(WindowsApp::OnUpdate);
	DXUTSetCallbackDeviceChanging(WindowsApp::ModifyDeviceSettings);

	if (g_pApp->m_Options.m_Renderer == "Direct3D 9")
	{
		DXUTSetCallbackD3D9DeviceAcceptable(WindowsApp::IsD3D9DeviceAcceptable);
		DXUTSetCallbackD3D9DeviceCreated(WindowsApp::OnD3D9CreateDevice);
		DXUTSetCallbackD3D9DeviceReset(WindowsApp::OnD3D9ResetDevice);
		DXUTSetCallbackD3D9DeviceLost(WindowsApp::OnD3D9LostDevice);
		DXUTSetCallbackD3D9DeviceDestroyed(WindowsApp::OnD3D9DestroyDevice);
		DXUTSetCallbackD3D9FrameRender(WindowsApp::OnD3D9FrameRender);
	}
	else if (g_pApp->m_Options.m_Renderer == "Direct3D 11")
	{
		DXUTSetCallbackD3D11DeviceAcceptable(WindowsApp::IsD3D11DeviceAcceptable);
		DXUTSetCallbackD3D11DeviceCreated(WindowsApp::OnD3D11CreateDevice);
		DXUTSetCallbackD3D11SwapChainResized(WindowsApp::OnD3D11ResizedSwapChain);
		DXUTSetCallbackD3D11SwapChainReleasing(WindowsApp::OnD3D11ReleasingSwapChain);
		DXUTSetCallbackD3D11DeviceDestroyed(WindowsApp::OnD3D11DestroyDevice);
		DXUTSetCallbackD3D11FrameRender(WindowsApp::OnD3D11FrameRender);
	}
	else
	{
		CB_ASSERT(0 && "Unknown renderer specified in game options.");
		return false;
	}

	// show the cursor and clip it when in full screen
	DXUTSetCursorSettings(true, true);

	// initialize the application
	if (!g_pApp->InitInstance(hInstance, lpCmdLine, hWnd, screenWidth, screenHeight))
	{
		return FALSE;
	}

	// C# will control the main loop, do no call to DXUTMainLoop()

	return true;
}


/*
	WndProc

	Call the WndProc callback function function directly.
*/
void WndProc(int* hWndPtrAddress, int msg, int wParam, int lParam)
{
	HWND hWnd = (HWND)hWndPtrAddress;
	DXUTStaticWndProc(hWnd, msg, WPARAM(wParam), LPARAM(lParam));
}


/*
	RenderFrame

	Tell the engine to render a frame.
*/
void RenderFrame()
{
	DXUTRender3DEnvironment();
}


/*
	Shutdown

	Shutdown the engine.
*/
int Shutdown()
{
	DXUTShutdown();
	return g_pApp->GetExitCode();
}


/*
	IsGameRunning

	Return whether the editor is running or not.
*/
int IsGameRunning()
{
	EditorLogic* game = (EditorLogic*)g_pApp->m_pGame;
	return game ? game->IsRunning() : false;
}


/*
	OpenLevel

	Open a level for editing.
*/
void OpenLevel(BSTR fullPathLevelFile)
{
	// strip off project directory
	std::string levelFile = ws2s(std::wstring(fullPathLevelFile, SysStringLen(fullPathLevelFile)));

	EditorLogic* pEditorLogic = (EditorLogic*)g_pApp->m_pGame;
	if (pEditorLogic)
	{
		std::string assetsDir = "\\Assets\\";
		int projDirLength = pEditorLogic->GetProjectDirectory().length() + assetsDir.length();
		g_pApp->m_Options.m_Level = levelFile.substr(projDirLength, levelFile.length() - projDirLength);
		pEditorLogic->ChangeState(BaseGameState::LoadingGameEnvironment);
	}
}


int GetNumGameObjects()
{
	EditorLogic* pGame = (EditorLogic*)g_pApp->m_pGame;
	return pGame ? pGame->GetNumGameObjects() : 0;
}


void GetGameObjectList(int* gameObjectIdArrayPtrAddress, int numGameObjects)
{
	// we return the id's of the game objects to the C#
	// application. it can then call back into the dll
	// to get info about each game object
	EditorLogic* pGame = (EditorLogic*)g_pApp->m_pGame;
	if (pGame)
	{
		auto gameObjectMap = pGame->GetGameObjectMap();

		for (GameObjectMap::const_iterator it = gameObjectMap.begin(), int index = 0;
			it != gameObjectMap.end() && index < numGameObjects; ++it, index++)
		{
			GameObjectId id = it->first;
			gameObjectIdArrayPtrAddress[index] = id;
		}
	}
}


int GetGameObjectXmlSize(GameObjectId gameObjectId)
{
	StrongGameObjectPtr pGameObject = MakeStrongPtr(g_pApp->m_pGame->GetGameObject(gameObjectId));
	if (!pGameObject)
	{
		return 0;
	}

	std::string xml = pGameObject->ToXML();
	return xml.length();
}


void GetGameObjectXml(int* gameObjectXmlPtrAddress, GameObjectId gameObjectId)
{
	StrongGameObjectPtr pGameObject = MakeStrongPtr(g_pApp->m_pGame->GetGameObject(gameObjectId));
	if (!pGameObject)
	{
		return;
	}

	std::string xml = pGameObject->ToXML();
	strncpy_s(reinterpret_cast<char*>(gameObjectXmlPtrAddress), xml.length() + 1, xml.c_str(), xml.length());
}


int PickGameObject(int* hWndPtrAddress)
{
	HWND hWnd = (HWND)hWndPtrAddress;

	POINT ptCursor;
	GetCursorPos(&ptCursor);

	// Convert the screen coordinates of the mouse cursor into
	// coordinates relative to the client window
	ScreenToClient(hWnd, &ptCursor);
	RayCast rayCast(ptCursor);
	EditorLogic* pGame = (EditorLogic*)g_pApp->m_pGame;

	if (!pGame)
	{
		return INVALID_GAMEOBJECT_ID;
	}

	shared_ptr<EditorHumanView> pView = pGame->GetHumanView();
	if (!pView)
	{
		return INVALID_GAMEOBJECT_ID;
	}


	// Cast a ray through the scene. The RayCast object contains an array of Intersection
	// objects.
	pView->GetScene()->Pick(&rayCast);
	rayCast.Sort();

	// If there are any intersections, get information from the first intersection.
	if (!rayCast.m_NumIntersections)
	{
		return INVALID_GAMEOBJECT_ID;
	}

	Intersection firstIntersection = rayCast.m_IntersectionArray[0];
	return firstIntersection.m_ObjectId;
}


int CreateGameObject(BSTR bstrActorResource)
{
	std::string gameObjectResource = ws2s(std::wstring(bstrActorResource, SysStringLen(bstrActorResource)));
	StrongGameObjectPtr pGameObject = g_pApp->m_pGame->CreateGameObject(gameObjectResource, NULL);
	if (!pGameObject)
		return INVALID_GAMEOBJECT_ID;

	// fire an event letting everyone else know that we created a new actor
	shared_ptr<Event_NewGameObject> pEvent(CB_NEW Event_NewGameObject(pGameObject->GetId()));
	IEventManager::Get()->QueueEvent(pEvent);
	return pGameObject->GetId();
}


void ModifyGameObject(BSTR bstrActorModificationXML)
{
	std::string actorModificationXML = ws2s(std::wstring(bstrActorModificationXML, SysStringLen(bstrActorModificationXML)));

	TiXmlDocument doc;
	doc.Parse(actorModificationXML.c_str());
	TiXmlElement* pRoot = doc.RootElement();
	if (!pRoot)
		return;

	g_pApp->m_pGame->ModifyGameObject(atoi(pRoot->Attribute("id")), pRoot);
}


void DestroyGameObject(GameObjectId gameObjectId)
{
	g_pApp->m_pGame->DestroyGameObject(gameObjectId);
}


 int GetLevelScriptAdditionsXmlSize()
{
	 TiXmlDocument* optionsDoc = g_pApp->m_Options.m_pDoc;
	 TiXmlElement* pRoot = optionsDoc->RootElement();
	 if (!pRoot)
		 return 0;

	 TiXmlElement* worldScriptAdditions = pRoot->FirstChildElement("WorldScriptAdditions");

	 TiXmlDocument outDoc;
	 TiXmlNode* node = worldScriptAdditions->Clone();
	 outDoc.LinkEndChild(node);

	 TiXmlPrinter printer;
	 outDoc.Accept(&printer);

	 std::string xml = printer.CStr();

	 return xml.length();
}


void GetLevelScriptAdditionsXml(int* xmlPtrAddress)
{
	TiXmlDocument* optionsDoc = g_pApp->m_Options.m_pDoc;
	TiXmlElement* pRoot = optionsDoc->RootElement();
	if (!pRoot)
		return;

	TiXmlElement* worldScriptAdditions = pRoot->FirstChildElement("WorldScriptAdditions");

	TiXmlDocument outDoc;
	TiXmlNode* node = worldScriptAdditions->Clone();
	outDoc.LinkEndChild(node);


	TiXmlPrinter printer;
	outDoc.Accept(&printer);

	std::string xml = printer.CStr();

	strncpy_s(reinterpret_cast<char *>(xmlPtrAddress), xml.length() + 1, xml.c_str(), xml.length());
}
