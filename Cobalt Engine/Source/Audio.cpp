/*
	Audio.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Audio.h"

Audio* g_pAudio = nullptr;

Audio::Audio() :
m_Initialized(false),
m_AllPaused(false)
{
}

Audio::~Audio()
{
	Shutdown();
}

void Audio::Shutdown()
{
	AudioBufferList::iterator it = m_AllSamples.begin();

	// stop and remove all sounds in the list
	while (it != m_AllSamples.end())
	{
		IAudioBuffer* audioBuffer = *it;
		audioBuffer->Stop();
		m_AllSamples.pop_front();
	}
}

void Audio::StopAllSounds()
{
	// stop all the sounds in the list
	for (AudioBufferList::iterator it = m_AllSamples.begin(); it != m_AllSamples.end(); ++it)
	{
		(*it)->Stop();
	}

	m_AllPaused = false;
}

void Audio::PauseAllSounds()
{
	// pause all the sounds in the list
	for (AudioBufferList::iterator it = m_AllSamples.begin(); it != m_AllSamples.end(); ++it)
	{
		(*it)->Pause();
	}

	m_AllPaused = true;
}

void Audio::ResumeAllSounds()
{
	// resume all the sounds in the list
	for (AudioBufferList::iterator it = m_AllSamples.begin(); it != m_AllSamples.end(); ++it)
	{
		(*it)->Resume();
	}

	m_AllPaused = false;
}

bool Audio::HasSoundCard()
{
	// return whether the sound sytem is active
	return g_pAudio && g_pAudio->Active();
}

bool Audio::IsPaused()
{
	return m_AllPaused;
}