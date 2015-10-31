/*
	ProcessManager.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <list>

#include "Process.h"

/**
	Manages all the running processes in a game. This class will call OnUpdate on
	each running process once per frame.
*/
class ProcessManager
{
	typedef std::list<StrongProcessPtr> ProcessList;
	
public:
	/// Default destructor
	~ProcessManager();

	/// Update all processes in the process list
	unsigned int UpdateProcesses(float deltaTime);

	/// Attach a process to the process manager
	WeakProcessPtr AttachProcess(StrongProcessPtr pProcess);

	/// Abort all processes in the process list
	void AbortAllProcesses(bool immediate);

	/// Return the number of processes in the process list
	unsigned int GetProcessCount() const;

private:
	/// Clears the process list -- called by the destructor
	void ClearAllProcesses();

private:
	/// List of all processes currently in queue
	ProcessList m_ProcessList;
};
