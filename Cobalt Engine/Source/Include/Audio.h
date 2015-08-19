/*
	Audio.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <list>
#include <memory>

#include "interfaces.h"
#include "ResourceHandle.h"

/**
	Platform agnostic sound system that sits between a platform specific
	sound system and the interface.
*/
class Audio : public IAudio
{
public:
	/// Default constructor
	Audio();

	/// Default destructor calls shutdown
	virtual ~Audio();

	/// Shutdown the sound system
	virtual void Shutdown() = 0;

	/// Stop all currently playing sounds
	virtual void StopAllSounds() = 0;

	/// Pause all currently playing sounds
	virtual void PauseAllSounds() = 0;

	/// Resume all paused sounds
	virtual void ResumeAllSounds() = 0;

	static bool HasSoundCard();

	/// Is the sound system paused?
	bool IsPaused();

protected:
	typedef std::list<IAudioBuffer*> AudioBufferList;

	/// List of all currently allocated sound buffers (one for each sound playing, even if theyre duplicates)
	AudioBufferList m_AllSamples;

	/// Is the sound system paused?
	bool m_AllPaused;

	/// Has the sound system been initialized?
	bool m_Initialized;
};

/// Global pointer to the sound system
extern Audio* g_pAudio;