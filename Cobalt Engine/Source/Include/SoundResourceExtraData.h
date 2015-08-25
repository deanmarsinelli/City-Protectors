/*
	SoundResourceExtraData.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <dsound.h>
#include <mmsystem.h>
#include <string>

#include "interfaces.h"

/// Types of sounds that can exist as resources
enum SoundType
{
	SOUND_TYPE_FIRST,
	SOUND_TYPE_MP3 = SOUND_TYPE_FIRST,
	SOUND_TYPE_WAVE,
	SOUND_TYPE_MIDI,
	SOUND_TYPE_OGG,

	// This needs to be the last sound type
	SOUND_TYPE_COUNT,
	SOUND_TYPE_UNKNOWN,
};

/**
	Extra data that is attached to a sound resource including type 
	of sound, length, etc.
*/
class SoundResourceExtraData : public IResourceExtraData
{
	friend class WaveResourceLoader;
	friend class OggResourceLoader;

public:
	/// Default constructor
	SoundResourceExtraData();

	/// Virtual destructor
	virtual ~SoundResourceExtraData() { }

	/// Return the string describing this extra data
	virtual std::string ToString();

	/// Return the type of sound resource
	SoundType GetSoundType() const;

	/// Return the PCM format of the sound resource
	const WAVEFORMATEX* GetFormat() const;

	/// Return the length of the sound in milliseconds
	int GetLengthMilli() const;

protected:
	/// Type of sound resource
	SoundType m_SoundType;

	/// Has the sound been initialized
	bool m_Initialized;

	/// Description of the PCM format
	WAVEFORMATEX m_WavFormatEx;

	/// Length of the sound in milliseconds
	int m_LengthMilliseconds;
};
