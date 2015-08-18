/*
	SoundResourceExtraData.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "SoundResourceExtraData.h"

SoundResourceExtraData::SoundResourceExtraData() :
m_SoundType(SoundType::SOUND_TYPE_UNKNOWN),
m_Initialized(false),
m_LengthMilliseconds(0)
{ }

std::string SoundResourceExtraData::ToString()
{
	return "SoundResourceExtraData";
}

enum SoundType SoundResourceExtraData::GetSoundType() const
{
	return m_SoundType;
}

const WAVEFORMATEX* SoundResourceExtraData::GetFormat() const
{
	return &m_WavFormatEx;
}

int SoundResourceExtraData::GetLengthMilli() const
{
	return m_LengthMilliseconds;
}
