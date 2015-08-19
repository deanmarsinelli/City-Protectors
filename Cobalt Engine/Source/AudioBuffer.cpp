/*
	AudioBuffer.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "AudioBuffer.h"

AudioBuffer::AudioBuffer(shared_ptr<ResHandle> resource)
{
	m_Resource = resource;
	m_IsPaused = false;
	m_IsLooping = false;
	m_Volume = 0;
}

shared_ptr<ResHandle> AudioBuffer::GetResource()
{
	return m_Resource;
}

bool AudioBuffer::IsLooping() const
{
	return m_IsLooping;
}

int AudioBuffer::GetVolume() const
{
	return m_Volume;
}
