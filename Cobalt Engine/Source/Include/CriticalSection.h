/*
	CriticalSection.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <windows.h>

/**
	This is a class wrapper for a windows critical section object
	to allow atomic use during multithreaded code.
*/
class CriticalSection
{
public:
	/// Initialize the critical section
	CriticalSection()
	{
		InitializeCriticalSection(&m_CS);
	}

	/// Destroy the critical section
	~CriticalSection()
	{
		DeleteCriticalSection(&m_CS);
	}

	/// Lock the critical section
	void Lock()
	{
		EnterCriticalSection(&m_CS);
	}

	/// Unlock the critical section
	void Unlock()
	{
		LeaveCriticalSection(&m_CS);
	}

private:
	/// The windows CRITICAL_SECTION object
	mutable CRITICAL_SECTION m_CS;
};
