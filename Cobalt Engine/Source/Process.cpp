/**
	Process.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Process.h"
#include "Logger.h"

Process::Process()
{
	m_State = UNINITIALIZED;
	m_pChild = nullptr;
}

Process::~Process()
{
	if (m_pChild)
	{
		m_pChild->OnAbort();
	}
}

inline void Process::Succeed()
{
	CB_ASSERT(m_State == RUNNING || m_State == PAUSED);
	m_State = SUCCEEDED;
}

inline void Process::Fail()
{
	CB_ASSERT(m_State == RUNNING || m_State == PAUSED);
	m_State = FAILED;
}

inline void Process::Pause()
{
	if (m_State == RUNNING)
	{
		m_State = PAUSED;
	}
	else
	{
		CB_WARNING("Attempting to pause a process that is not running");
	}
}

inline void Process::UnPause()
{
	if (m_State == PAUSED)
	{
		m_State = RUNNING;
	}
	else
	{
		CB_WARNING("Attempting to unpase a process that is not paused")
	}
}

inline Process::State Process::GetState() const
{
	return m_State;
}

inline bool Process::IsAlive() const
{
	return (m_State == RUNNING || m_State == PAUSED);
}

inline bool Process::IsDead() const
{
	return (m_State == SUCCEEDED || m_State == FAILED || m_State == ABORTED);
}

inline bool Process::IsRemoved() const
{
	return m_State == REMOVED;
}

inline bool Process::IsPaused() const
{
	return m_State == PAUSED;
}

inline void Process::AttachChild(StrongProcessPtr pChild)
{
	if (m_pChild)
	{
		m_pChild->AttachChild(pChild);
	}
	else
	{
		m_pChild = pChild;
	}
}

StrongProcessPtr Process::RemoveChild()
{
	if (m_pChild)
	{
		StrongProcessPtr pChild = m_pChild;
		m_pChild.reset();
		return pChild;
	}

	return nullptr;
}

inline StrongProcessPtr Process::PeekChild()
{
	return m_pChild;
}

inline void Process::SetState(State newState)
{
	m_State = newState;
}