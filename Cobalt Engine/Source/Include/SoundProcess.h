/*
	SoundProcess.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <memory>

#include "interfaces.h"
#include "Process.h"
#include "ResourceHandle.h"

/**
	This class manages an instance of a sound. It inherits from the process class and
	is managed by the process manager allowing it to execute over multiple frames, and 
	clean itself up when the sound is finished. A single sound resource exists in the
	resource cache, but can have many SoundProcess instances.

	This class can be subclassed by specific sounds that are frequently used.
*/
class SoundProcess : public Process
{
public:
	/// Constructor taking in a sound resource handle
	SoundProcess(shared_ptr<ResHandle> soundResource, int volume = 100, bool looping = false);
	
	/// Virtual destructor
	virtual ~SoundProcess();

	/// Play a sound
	void Play(const int volume, const bool looping);

	/// Stop the sound from playing
	void Stop();

	/// Set the sound volume
	void SetVolume(int volume);

	/// Get the sound volume
	int GetVolume();

	/// Get the length of the sound in milliseconds
	int GetLengthMilli();

	/// Return whether or not the sound resource handle is valid
	bool IsSoundValid();

	/// Is the sound currently playing?
	bool IsPlaying();

	/// Is the sound set to loop?
	bool IsLooping();

	/// Return how much of the sound has played
	float GetProgress();

	/// Pause the sound if it is playing
	void PauseSound();

protected:
	/// Initialize the sound
	virtual void OnInit();

	/// Update method called once per frame
	virtual void OnUpdate(const float deltaTime);

	/// Initialize the volume of the sound
	void InitializeVolume();

protected:
	/// Handle to the raw sound data resource that lives in the cache (one resource exists per sound)
	shared_ptr<ResHandle> m_Handle;

	/// Handle to the implementation dependent audio buffer (a new buffer exists for every sound that plays including duplicates)
	shared_ptr<IAudioBuffer> m_AudioBuffer;

	/// The sounds volume
	int m_Volume;

	/// Is the sound looping?
	bool m_IsLooping;
};

typedef SoundProcess SoundEffect;
