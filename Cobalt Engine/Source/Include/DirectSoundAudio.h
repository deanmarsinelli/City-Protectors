/*
	DirectSoundAudio.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <dsound.h>

#include "Audio.h"
#include "interfaces.h"
#include "ResourceHandle.h"

/**
	A DirectSound implementation of an audio system. This class acts as 
	a C++ wrapper for DirectSound8.
*/
class DirectSoundAudio : public Audio
{
public:
	/// Default constructor
	DirectSoundAudio();

	/// Initialize the sound system
	virtual bool Initialize(HWND hWnd);

	/// Shutdown the sound system
	virtual void Shutdown();

	/// Return true if the sound system is active
	virtual bool Active();

	/// Initialize an audio buffer
	virtual IAudioBuffer* InitAudioBuffer(shared_ptr<ResHandle> soundResource);

	/// Release an audio buffer
	virtual void ReleaseAudioBuffer(IAudioBuffer* audioBuffer);

protected:
	HRESULT SetPrimaryBufferFormat(DWORD primaryChannels, DWORD primaryFreq, DWORD primaryBitRate);

protected:
	/// Pointer to direct sound 
	IDirectSound8* m_pDS;
};
