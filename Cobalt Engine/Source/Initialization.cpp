/*
	Initialization.cpp

	Functions definitions for intialiazing
	a game instance.

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <direct.h>
#include <ShlObj.h>
#include <tchar.h>

#include "EngineStd.h"
#include "Initialization.h"
#include "Logger.h"
#include "types.h"

/*
	IsOnlyInstance - WINDOWS SPECIFIC
	Return whether this instance is the only instance of the game
*/
bool IsOnlyInstance(LPCTSTR gameTitle)
{
	// find the game window if it already exists
	// and set it to active
	CreateMutex(NULL, TRUE, gameTitle);

	if (GetLastError() != ERROR_SUCCESS)
	{
		HWND hWnd = FindWindow(gameTitle, NULL);
		if (hWnd)
		{
			// instance is already running
			ShowWindow(hWnd, SW_SHOWNORMAL);
			SetFocus(hWnd);
			SetForegroundWindow(hWnd);
			SetActiveWindow(hWnd);

			return false;
		}
	}
	return true;
}

/*
	CheckStorage - return whether the system has enough disk space
*/
bool CheckStorage(const DWORDLONG diskSpaceNeeded)
{
	// _getdrive() works on any ANSI compatible system
	int const drive = _getdrive();
	struct _diskfree_t diskfree;

	// returns the amount of free clusters
	_getdiskfree(drive, &diskfree);

	// convert needed bytes to needed clusters
	unsigned __int64 const neededClusters = diskSpaceNeeded /
		(diskfree.sectors_per_cluster * diskfree.bytes_per_sector);

	if (diskfree.avail_clusters < neededClusters)
	{
		//ERROR MESSAGE
		return false;
	}
	return true;
}

/*
	CheckMemory - WINDOWS SPECIFIC
	Returns whether the system has the required memory
*/
bool CheckMemory(DWORDLONG physicalRamNeeded, const DWORDLONG virtualRAMNeeded)
{
	// get the amount of system ram
	MEMORYSTATUSEX status;
	GlobalMemoryStatusEx(&status);

	if (status.ullTotalPhys < physicalRamNeeded)
	{
		// not enough memory, error message
		return false;
	}
	if (status.ullAvailVirtual < virtualRAMNeeded)
	{
		// not enough virtual memory, error message
		return false;
	}

	// make sure there is enough ram in one block
	unsigned char* buffer = (unsigned char*)malloc((size_t)(virtualRAMNeeded * sizeof(unsigned char)));
	if (buffer)
	{
		free(buffer);
	}
	else
	{
		// not enough contiguous memory
		return false;
	}
	return true;
}

/*
	ReadCPUSpeed - WINDOWS SPECIFIC
	Returns the cpu speed
*/
DWORD ReadCPUSpeed()
{
	DWORD bufferSize = sizeof(DWORD);
	DWORD bufferMHz = 0;
	DWORD type = REG_DWORD;
	HKEY hKey;

	// read cpu speed from the registry
	long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);
	if (lError == ERROR_SUCCESS)
	{
		RegQueryValueEx(hKey, L"~MHz", NULL, &type, (LPBYTE)&bufferMHz, &bufferSize);
	}

	return bufferMHz;
}

/*
	GetSaveGameDirectory
	Returns the directory to store game files
*/
const TCHAR* GetSaveGameDirectory(HWND hWnd, const TCHAR* gameAppDirectory)
{
	HRESULT hr;
	static TCHAR saveGameDirectory[MAX_PATH];
	TCHAR userDataPath[MAX_PATH];
	hr = SHGetSpecialFolderPath(hWnd, userDataPath, CSIDL_APPDATA, true);

	_tcscpy_s(saveGameDirectory, userDataPath);
	_tcscat_s(saveGameDirectory, L"\\");
	_tcscat_s(saveGameDirectory, gameAppDirectory);

	// check if directory exists
	if (0xffffffff == GetFileAttributes(saveGameDirectory))
	{
		if (SHCreateDirectoryEx(hWnd, saveGameDirectory, nullptr) != ERROR_SUCCESS)
		{
			return false;
		}
	}

	_tcscat_s(saveGameDirectory, L"\\");

	return saveGameDirectory;
}

GameOptions::GameOptions()
{
	// set all options to default values
	m_Level = "";
	m_Renderer = "Direct3D 9";
	m_RunFullSpeed = false;
	m_SoundEffectsVolume = 1.0f;
	m_MusicVolume = 1.0f;
	m_ExpectedPlayers = 1;
	m_ListenPort = -1;
	m_GameHost = "Nobody";
	m_NumAIs = 1;
	m_MaxAIs = 4;
	m_MaxPlayers = 4;
	m_ScreenSize = Point(1024, 768);
	m_UseDevelopmentDirectories = false;
	m_pDoc = nullptr;
}

GameOptions::~GameOptions()
{
	CB_SAFE_DELETE(m_pDoc);
}

void GameOptions::Init(const char* xmlFilePath, LPWSTR lpCmdLine)
{
	// load options from xml file
	m_pDoc = new TiXmlDocument(xmlFilePath);
	if (m_pDoc && m_pDoc->LoadFile())
	{
		TiXmlElement* pRoot = m_pDoc->RootElement();
		if (!pRoot)
		{
			return;
		}

		TiXmlElement* pNode = nullptr;
		pNode = pRoot->FirstChildElement("Graphics");
		if (pNode)
		{
			std::string attribute;
			attribute = pNode->Attribute("renderer");
			if (attribute != "Direct3D 9" && attribute != "Direct3D 11")
			{
				CB_ASSERT(0 && "Bad renderer setting in graphics options");
			}
			else
			{
				m_Renderer = attribute;
			}

			if (pNode->Attribute("width"))
			{
				m_ScreenSize.x = atoi(pNode->Attribute("width"));
				if (m_ScreenSize.x < 800)
					m_ScreenSize.x = 800;
			}

			if (pNode->Attribute("height"))
			{
				m_ScreenSize.y = atoi(pNode->Attribute("height"));
				if (m_ScreenSize.x < 600)
					m_ScreenSize.x = 600;
			}

			if (pNode->Attribute("runfullspeed"))
			{
				attribute = pNode->Attribute("runfullspeed");
				m_RunFullSpeed = (attribute == "yes") ? true : false;
			}
		}

		pNode = pRoot->FirstChildElement("Sound");
		if (pNode)
		{
			m_MusicVolume = atoi(pNode->Attribute("musicVolume")) / 100.0f;
			m_SoundEffectsVolume = atoi(pNode->Attribute("sfxVolume")) / 100.0f;
		}

		pNode = pRoot->FirstChildElement("Multiplayer");
		if (pNode)
		{
			m_ExpectedPlayers = atoi(pNode->Attribute("expectedPlayers"));
			m_NumAIs = atoi(pNode->Attribute("numAIs"));
			m_MaxAIs = atoi(pNode->Attribute("maxAIs"));
			m_MaxPlayers = atoi(pNode->Attribute("maxPlayers"));
			m_ListenPort = atoi(pNode->Attribute("listenPort"));
			m_GameHost = pNode->Attribute("gameHost");
		}

		pNode = pRoot->FirstChildElement("ResCache");
		if (pNode)
		{
			std::string attribute(pNode->Attribute("useDevelopmentDirectories"));
			m_UseDevelopmentDirectories = (attribute == "yes") ? true : false;
		}
	}
}
