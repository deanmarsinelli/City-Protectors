/*
	RealTimeProcess.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <Windows.h>

#include "Process.h"

/**
	A real time process is a process that runs asynchronously on
	its own thread.
*/
class RealTimeProcess : public Process
{
public:
	/// Constructor sets the thread priority
	RealTimeProcess(int priority = THREAD_PRIORITY_NORMAL);

	/// Virtual destructor closes the thread handle
	virtual ~RealTimeProcess() { CloseHandle(m_ThreadHandle); }

	/// Procedure that runs on a separate thread. Pass a RealTimeProcess object
	/// as the parameter and this procedure calls the implementation
	/// specific ProcessProc() from that process
	static DWORD WINAPI ThreadProc(LPVOID lpParam);

protected:
	/// Initialize the Process
	virtual void OnInit();

	/// Update method (does nothing since this runs asynchronously)
	virtual void OnUpdate(const float deltaTime) { }

	/// This is the procedure that will run on a separate thread and is
	/// implementation specific for each real time process. It is called
	/// by ThreadProc()
	virtual void ProcessProc() = 0; 

protected:
	/// A handle to the windows thread
	HANDLE m_ThreadHandle;

	/// Id of the thread
	DWORD m_ThreadId;

	/// Priority of the thread
	int m_ThreadPriority;
};
