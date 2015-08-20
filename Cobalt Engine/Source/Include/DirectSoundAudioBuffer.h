/*
	DirectSoundAudioBuffer.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <dsound.h>
#include <memory>

#include "AudioBuffer.h"
#include "ResourceHandle.h"

class DirectSoundAudioBuffer : public AudioBuffer
{
public:
	/// Constructor taking in a sample and a resource handle
	DirectSoundAudioBuffer(LPDIRECTSOUNDBUFFER sample, shared_ptr<ResHandle> resource);
	
	// Default destructor frees memory
	virtual ~DirectSoundAudioBuffer();

	/// Return a pointer to the direct sound buffer
	virtual void* Get();

	/// Restore an audio buffer that has been lost
	virtual bool OnRestore();

	/// Play an audio sound, volume should be 0 - 100
	virtual bool Play(int volume, bool looping);

	/// Pause a sound that is currently playing
	virtual bool Pause();

	/// Stop a sound that is playing
	virtual bool Stop();

	/// Resume a paused sound
	virtual bool Resume();

	/// Pause or unpause a sound based on the current state of the sound
	virtual bool TogglePause();

	/// Return true if the sound is currently playing
	virtual bool IsPlaying();

	/// Set the volume of the sound
	virtual void SetVolume(int volume);

	/// Instantly set the sound to a new position
	virtual void SetPosition(unsigned long newPosition);

	/// Return a value between 0.0 and 1.0 that represents how much of a sound has played
	virtual float GetProgress();

private:
	/// Copy data from a sound resource into a DirectSound buffer
	HRESULT FillBufferWithSound();
	HRESULT RestoreBuffer(BOOL* pWasRestored);

protected:
	/// Direct sound buffer - each sound that plays will have this buffer, even 
	/// multiple copies of a single sound resource
	LPDIRECTSOUNDBUFFER m_Sample;
};