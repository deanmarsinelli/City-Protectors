/*
	DelayedProcess.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "Process.h"

/**
	A Delayed Process is used as a parent process for a process that you want to run
	after a set amount of time. A delayed process will succeed after a given time 
	interval, and your process (added as a child to the delayed process) will be 
	promoted to a full process and execute its logic.
*/
class DelayedProcess : public Process
{
public:
	/// Constructor taking a delay timer
	explicit DelayedProcess(const float delayTimer) :
		Process(),
		m_DelayTimer(delayTimer),
		m_ElapsedTime(0)
	{}

protected:
	/// Update function will succeed after the delay timer
	virtual void OnUpdate(const float deltaTime)
	{
		m_ElapsedTime += deltaTime;
		if (m_ElapsedTime >= m_DelayTimer)
			Succeed();
	}

private:
	/// The amount of time to delay before success
	const float m_DelayTimer;

	/// Time elapsed since the process was created
	float m_ElapsedTime;
};