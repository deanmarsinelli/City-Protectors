/*
	Initialization.h

	Functions declarations for intialiazing
	a game instance.

	GameOptions struct used as a container
	for various game options.

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <string>
#include "EngineStd.h"
#include "types.h"

// external function declarations
// these don't have to be declared extern as all functions
// have global scope in C, but it let's you know the definition
// is in another file Initialization.cpp
extern bool IsOnlyInstance(LPCTSTR gameTitle);
extern bool CheckStorage(const DWORDLONG diskSpaceNeeded);
extern bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded);
extern DWORD ReadCPUSpeed();
extern const TCHAR* GetSaveGameDirectory(HWND hWnd, const TCHAR* gameAppDirectory);
extern bool CheckForJoystick(HWND hWnd);

/**
	Options to initialize the game.
*/
struct GameOptions
{
	GameOptions();
	~GameOptions() { CB_SAFE_DELETE(m_pDoc); }

	void Init(const char* xmlFilePath, LPWSTR lpCmdLine);

	// level option
	std::string m_Level;

	// rendering options
	std::string m_Renderer;
	bool m_RunFullSpeed;
	Point m_ScreenSize;

	// sound options
	float m_SoundEffectsVolume;
	float m_MusicVolume;
	float m_DialogueVolume;

	// multiplayer options
	int m_ExpectedPlayers;
	int m_ListenPort;
	std::string m_GameHost;
	int m_NumAIs;
	int m_MaxAIs;
	int m_MaxPlayers;

	// resource cache options
	bool m_UseDevelopmentDirectories;

	// xml options document
	TiXmlDocument* m_pDoc;
};