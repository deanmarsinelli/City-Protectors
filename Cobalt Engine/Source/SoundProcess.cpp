/*
	SoundProcess.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "SoundProcess.h"

#include "Audio.h"
#include "Logger.h"
#include "SoundResourceExtraData.h"

SoundProcess::SoundProcess(shared_ptr<ResHandle> soundResource, int volume, bool looping) :
m_Handle(soundResource),
m_Volume(volume),
m_IsLooping(looping)
{
	InitializeVolume();
}

SoundProcess::~SoundProcess()
{
	if (IsPlaying())
		Stop();

	if (m_AudioBuffer)
		g_pAudio->ReleaseAudioBuffer(m_AudioBuffer.get());
}

void SoundProcess::Play(const int volume, const bool looping)
{
	CB_ASSERT(volume >= 0 && volume <= 100 && "Volume must be between 0 and 100");

	if (m_AudioBuffer)
	{
		// tell the sound to play
		m_AudioBuffer->Play(volume, looping);
	}
}

void SoundProcess::Stop()
{
	if (m_AudioBuffer)
	{
		m_AudioBuffer->Stop();
	}
}

void SoundProcess::SetVolume(int volume)
{
	if (!m_AudioBuffer)
	{
		return;
	}

	CB_ASSERT(volume >= 0 && volume <= 100 && "Volume must be between 0 and 100");
}

int SoundProcess::GetVolume()
{
	if (!m_AudioBuffer)
	{
		return 0;
	}

	m_Volume = m_AudioBuffer->GetVolume();
	return m_Volume;
}

int SoundProcess::GetLengthMilli()
{
	// if the handle has proper sound extra data, return the length
	if (m_Handle && m_Handle->GetExtra())
	{
		shared_ptr<SoundResourceExtraData> extra = static_pointer_cast<SoundResourceExtraData>(m_Handle->GetExtra());
		return extra->GetLengthMilli();
	}
	else
	{
		return 0;
	}
}

bool SoundProcess::IsSoundValid()
{
	return m_Handle != nullptr;
}

bool SoundProcess::IsPlaying()
{
	if (!m_Handle || !m_AudioBuffer)
	{
		return false;
	}

	return m_AudioBuffer->IsPlaying();
}

bool SoundProcess::IsLooping()
{
	return m_AudioBuffer && m_AudioBuffer->IsLooping();
}

float SoundProcess::GetProgress()
{
	if (m_AudioBuffer)
	{
		return m_AudioBuffer->GetProgress();
	}

	return 0.0f;
}

void SoundProcess::PauseSound()
{
	if (m_AudioBuffer)
	{
		m_AudioBuffer->TogglePause();
	}
}

void SoundProcess::OnInit()
{
	// call base class init to set state to RUNNING
	Process::OnInit();

	// make sure the handle has valid extra sound data
	if (!m_Handle || m_Handle->GetExtra())
		return;

	// initialize the sound in the audio engine
	IAudioBuffer* buffer = g_pAudio->InitAudioBuffer(m_Handle);

	if (!buffer)
	{
		Fail();
		return;
	}

	// store the raw audio buffer
	m_AudioBuffer.reset(buffer);
}

void SoundProcess::OnUpdate(const float deltaTime)
{
	// when the sound is done playing, call succeed
	if (!IsPlaying())
	{
		Succeed();
	}
}

void SoundProcess::InitializeVolume()
{
	// TODO: volume based on game options
}
