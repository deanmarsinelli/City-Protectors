/**
	ProcessManager.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "ProcessManager.h"

ProcessManager::~ProcessManager()
{
	ClearAllProcesses();
}

unsigned int ProcessManager::UpdateProcesses(unsigned long deltaTime)
{
	unsigned short int successCount = 0;
	unsigned short int failCount = 0;

	auto it = m_ProcessList.begin();
	while (it != m_ProcessList.end())
	{
		StrongProcessPtr pCurrentProcess = *it;
		
		// save an iterator to this process and increment our list iterator
		auto thisIt = it;
		++it;

		// init the current process if it has not intialized yet
		if (pCurrentProcess->GetState() == Process::UNINITIALIZED)
		{
			pCurrentProcess->OnInit();
		}

		// update a running process
		if (pCurrentProcess->GetState() == Process::RUNNING)
		{
			pCurrentProcess->OnUpdate(deltaTime);
		}

		// if a process is dead, run the correct exit method
		if (pCurrentProcess->IsDead())
		{
			switch (pCurrentProcess->GetState())
			{
			case Process::SUCCEEDED:
				{
					pCurrentProcess->OnSuccess();
					StrongProcessPtr pChild = pChild->RemoveChild();
					if (pChild)
						AttachProcess(pChild);
					else
						++successCount;
					break;
				}
			case Process::FAILED:
				{
					pCurrentProcess->OnFail();
					++failCount;
					break;
				}
			case Process::ABORTED:
				{
					pCurrentProcess->OnAbort();
					++failCount;
					break;
				}
			}

			// destroy the dead process
			m_ProcessList.erase(thisIt);
		}
	}

	return (successCount << 16) | failCount;
}

WeakProcessPtr ProcessManager::AttachProcess(StrongProcessPtr pProcess)
{
	m_ProcessList.push_front(pProcess);
	return WeakProcessPtr(pProcess);
}

void ProcessManager::AbortAllProcesses(bool immediate)
{
	auto it = m_ProcessList.begin();
	while (it != m_ProcessList.end())
	{
		auto tempIt = it;
		++it;

		StrongProcessPtr pProcess = *tempIt;
		if (pProcess->IsAlive())
		{
			pProcess->SetState(Process::ABORTED);
			// if immediate, call the abort code and erase the process now
			if (immediate)
			{
				pProcess->OnAbort();
				m_ProcessList.erase(tempIt);
			}
		}
	}
}

inline unsigned int ProcessManager::GetProcessCount() const
{
	return m_ProcessList.size();
}

void ProcessManager::ClearAllProcesses()
{
	m_ProcessList.clear();
}