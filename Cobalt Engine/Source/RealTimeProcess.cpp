/*
	RealTimeProcess.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Logger.h"
#include "RealTimeProcess.h"

RealTimeProcess::RealTimeProcess(int priority)
{
	m_ThreadId = 0;
	m_ThreadPriority = priority;
}

DWORD WINAPI RealTimeProcess::ThreadProc(LPVOID lpParam)
{
	// cast the param to a real time procedure and call the process proc
	RealTimeProcess *proc = static_cast<RealTimeProcess*>(lpParam);
	proc->ProcessProc();

	return TRUE;
}

void RealTimeProcess::OnInit()
{
	// call base class init
	Process::OnInit();

	// create a windows thread
	m_ThreadHandle = CreateThread(
		NULL,						// default security attributes
		0,							// default stack size
		ThreadProc,					// thread process
		this,						// parameter is a pointer to this object
		0,							// default creation flags
		&m_ThreadId);				// return the id of the thread

	if (m_ThreadHandle == nullptr)
	{
		CB_ERROR("Could not create thread");
		Fail();
		return;
	}

	SetThreadPriority(m_ThreadHandle, m_ThreadPriority);
}