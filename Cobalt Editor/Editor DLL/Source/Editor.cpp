/*
	Editor.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <direct.h>
#include <Logger.h>
#include <Physics.h>

#include "Editor.h"
#include "resource.h"

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")			// useful for Windows Registry queries
#pragma comment(lib, "OleAut32.lib")			// needed for the C# / C++ BSTR conversions

#pragma comment(lib, "d3dcompiler.lib")

#ifdef _DEBUG
#pragma comment(lib, "d3dx9d.lib")
#pragma comment(lib, "d3dx11d.lib")
#pragma comment(lib, "DXUTd.lib")
#pragma comment(lib, "DXUTOptd.lib")
#else
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "DXUT.lib")
#pragma comment(lib, "DXUTOpt.lib")
#endif

#pragma comment(lib, "DxErr.lib")
#pragma comment(lib, "Comctl32.lib")

#ifdef _DEBUG
	#pragma comment(lib, "bulletcollision_debug.lib")
	#pragma comment(lib, "bulletdynamics_debug.lib")
	#pragma comment(lib, "linearmath_debug.lib")
#else
	#pragma comment(lib, "bulletcollision.lib")
	#pragma comment(lib, "bulletdynamics.lib")
	#pragma comment(lib, "linearmath.lib")
#endif

#pragma comment(lib, "zlib.lib")

#ifdef _DEBUG
	#pragma comment(lib, "libogg_staticd.lib")
	#pragma comment(lib, "libvorbis_staticd.lib")
	#pragma comment(lib, "libvorbisfile_staticd.lib")
#else
	#pragma comment(lib, "libogg_static.lib")
	#pragma comment(lib, "libvorbis_static.lib")
	#pragma comment(lib, "libvorbisfile_static.lib")
#endif

#pragma comment(lib, "tinyxml.lib")


EditorApp globalApp;

EditorApp::EditorApp() : 
WindowsApp()
{
	m_IsEditorRunning = true;
}

TCHAR* EditorApp::GetGameTitle()
{
	return L"Cobalt Engine Editor";
}

TCHAR* EditorApp::GetGameAppDirectory()
{
	return L""; // TODO
}

HICON EditorApp::GetIcon()
{
	return LoadIcon(GetInstance(), MAKEINTRESOURCE(IDI_ICON1));
}

BaseGameLogic* EditorApp::CreateGameAndView()
{
	BaseGameLogic* game = NULL;

	game = CB_NEW EditorLogic();
	game->Init();

	shared_ptr<IGameView> gameView(CB_NEW EditorHumanView(g_pApp->m_Renderer));
	game->AddView(gameView);

	return game;
}



EditorLogic::EditorLogic() : 
BaseGameLogic()
{
	m_ProjectDirectory = _getcwd(NULL, 0);
	int slashGamePos = m_ProjectDirectory.rfind("\\Game");
	m_ProjectDirectory = m_ProjectDirectory.substr(0, slashGamePos);

	m_pPhysics.reset(CreateNullPhysics());
}

EditorLogic::~EditorLogic()
{

}

bool EditorLogic::LoadGame(const char* levelResource)
{
	while (m_Objects.size() > 0)
	{
		GameObjectId id = m_Objects.begin()->first;
		DestroyGameObject(id);
	}

	if (!BaseGameLogic::LoadGame(levelResource))
	{
		return false;
	}

	ChangeState(BaseGameState::Running);

	return true;
}

const std::string& EditorLogic::GetProjectDirectory()
{
	return m_ProjectDirectory;
}

int EditorLogic::GetNumGameObjects()
{
	return (int)m_Objects.size();
}

const GameObjectMap& EditorLogic::GetGameObjectMap()
{
	return m_Objects;
}

bool EditorLogic::IsRunning()
{
	return (m_State == BaseGameState::Running);
}

shared_ptr<EditorHumanView> EditorLogic::GetHumanView()
{
	CB_ASSERT(m_GameViews.size() == 1);
	shared_ptr<IGameView> pGameView = *m_GameViews.begin();
	shared_ptr<EditorHumanView> editorHumanView = static_pointer_cast<EditorHumanView>(pGameView);

	return editorHumanView;
}
