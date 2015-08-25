/*
	AudioBuffer.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <memory>

#include "interfaces.h"
#include "ResourceHandle.h"

/*
	Platform agnostic audio buffer that sits between the interface
	and the platform specific audio buffer.
*/
class AudioBuffer : public IAudioBuffer
{
public:
	/// Return a pointer to the resource handle of this audio sound
	virtual shared_ptr<ResHandle> GetResource();

	/// Return true if the sound is currently set to loop
	virtual bool IsLooping() const;

	/// Return the volume of the sound
	virtual int GetVolume() const;

protected:
	/// Constructor taking a handle to a resource, public construction is disabled
	AudioBuffer(shared_ptr<ResHandle> resource);

protected:
	/// Pointer to the sound's resource handle
	shared_ptr<ResHandle> m_Resource;

	/// Is the sound paused
	bool m_IsPaused;

	/// Is the sound looping
	bool m_IsLooping;

	/// Current volume of the sound
	int m_Volume;
};
