/*
	Process.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <memory>

class Process;
typedef std::shared_ptr<Process> StrongProcessPtr;
typedef std::weak_ptr<Process> WeakProcessPtr;

/**
	A Process simulates a running process. Processes are managed externally by a Process Manager
	and execute their logic in a chaing. A process can also have a child process which is a
	suspended process that will begin execution if its parent process completes successfully. 
	In this case, the child process will be promoted by the Process Manager to a full process and 
	execute its own behavior in the chain. If a child is added to a process that already has a 
	child, the new child (grand child) will be added as a child to the processes child.
*/
class Process
{
	friend class ProcessManager;

public:
	/// Describes the state that a process can be in
	enum State
	{
		UNINITIALIZED = 0,
		REMOVED,
		// running processes
		RUNNING,
		PAUSED,
		// finished processes
		SUCCEEDED,
		FAILED,
		ABORTED
	};

	/// Default constructor
	Process();

	/// Virtual destructor
	virtual ~Process();

	/// Method for ending a process with success
	void Succeed();

	/// Method for ending a process with failure
	void Fail();

	/// Pause a running process
	void Pause();

	/// Resume a paused process
	void UnPause();

	/// Return the state of a process
	State GetState() const;

	/// Return if a process is currently alive
	bool IsAlive() const;

	/// Return if a process is currently dead
	bool IsDead() const;

	/// Return if a process is removed
	bool IsRemoved() const;

	/// Return whether or not a process is paused
	bool IsPaused() const;

	/// Attach a child process to this process
	void AttachChild(StrongProcessPtr pChild);

	/// Remove the child process from this process
	StrongProcessPtr RemoveChild();

	/// Return a pointer to the child process
	StrongProcessPtr PeekChild();

protected:
	/// Default Init method sets the state to RUNNING
	virtual void OnInit()
	{
		m_State = RUNNING;
	}

	/// Update method must be overriden in derived class and is called every frame
	virtual void OnUpdate(float deltaTime) = 0;

	/// Called if a process ends with sucess
	virtual void OnSuccess() { }

	/// Called if a process ends with failure
	virtual void OnFail() { }

	/// Called if a process ends with abort
	virtual void OnAbort() { }

private:
	/// Sets the state of a process -- done by the manager
	void SetState(State newState);

private:
	/// Current State of the Process
	State m_State;

	/// Strong pointer to a child process, if one exists
	StrongProcessPtr m_pChild;
};
