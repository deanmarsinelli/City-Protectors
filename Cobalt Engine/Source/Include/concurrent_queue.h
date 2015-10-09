/*
	concurrent_queue.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "CriticalSection.h"

// concurrent_queue was grabbed from 
// http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
// and was written by Anthony Williams

template<typename Data>
class concurrent_queue
{
private:
	std::queue<Data> the_queue;
	CriticalSection m_cs;
	HANDLE m_dataPushed;
public:
	concurrent_queue()
	{
		m_dataPushed = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	void push(Data const& data)
	{
		{
			ScopedCriticalSection locker(m_cs);
			the_queue.push(data);
		}
		PulseEvent(m_dataPushed);
	}

	bool empty() const
	{
		ScopedCriticalSection locker(m_cs);
		return the_queue.empty();
	}

	bool try_pop(Data& popped_value)
	{
		ScopedCriticalSection locker(m_cs);
		if (the_queue.empty())
		{
			return false;
		}

		popped_value = the_queue.front();
		the_queue.pop();
		return true;
	}

	void wait_and_pop(Data& popped_value)
	{
		ScopedCriticalSection locker(m_cs);
		while (the_queue.empty())
		{
			WaitForSingleObject(m_dataPushed);
		}

		popped_value = the_queue.front();
		the_queue.pop();
	}
};
