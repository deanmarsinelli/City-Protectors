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


/**
	This is a helper class that allows automatic locking/unlocking
	of a critical section when the object is created or destroyed.

	Usage:
	Create the critical section object
	Create a scope and add the scoped critical section object

	CriticalSection cs;

	// unlocked code

	{
		ScopedCriticalSection this_is_locked(cs);

		// code that needs thread locking
	}

	The scoped critical section will be destroyed at the end of the scope
	there by unlocking the critical section object.
*/
class ScopedCriticalSection
{
public:
	ScopedCriticalSection(CriticalSection& csResource) :
		m_CSResource(csResource)
	{
		m_CSResource.Lock();
	}

	~ScopedCriticalSection()
	{
		m_CSResource.Unlock();
	}

private:
	CriticalSection &m_CSResource;
};
