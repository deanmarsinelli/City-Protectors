/*
	Editor.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <BaseGameLogic.h>
#include <WindowsApp.h>
#include <string>

#include "EditorHumanView.h"

/*
	EditorApp class is a child class of the WindowsApp class.
*/
class EditorApp : public WindowsApp
{
public:
	EditorApp();
	TCHAR* GetGameTitle();
	TCHAR* GetGameAppDirectory();
	HICON GetIcon() override;

protected:
	BaseGameLogic* CreateGameAndView() override;
};


/*
	EditorLogic class inherits from BaseGameLogic.
*/
class EditorLogic : public BaseGameLogic
{
public:
	EditorLogic();
	~EditorLogic();

	virtual bool LoadGame(const char* levelResource) override;
	const std::string& GetProjectDirectory();
	int GetNumGameObjects();
	const GameObjectMap& GetGameObjectMap();
	bool IsRunning();
	shared_ptr<EditorHumanView> GetHumanView();

protected:
	std::string m_ProjectDirectory;
};
