/*
	Initialization.cpp

	Functions definitions for intialiazing
	a game instance.

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Initialization.h"
#include <ShlObj.h>
#include <direct.h>
#include <tchar.h>

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