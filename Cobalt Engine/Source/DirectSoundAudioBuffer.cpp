/*
	DirectSoundAudioBuffer.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "DirectSoundAudioBuffer.h"

#include "Audio.h"
#include "Logger.h"
#include "SoundResourceExtraData.h"

DirectSoundAudioBuffer::DirectSoundAudioBuffer(LPDIRECTSOUNDBUFFER sample, shared_ptr<ResHandle> resource) :
AudioBuffer(resource)
{
	m_Sample = sample;
	FillBufferWithSound();
}

DirectSoundAudioBuffer::~DirectSoundAudioBuffer()
{
	CB_COM_RELEASE(m_Sample);
}

void* DirectSoundAudioBuffer::Get()
{
	if (!OnRestore())
		return nullptr;

	return m_Sample;
}

bool DirectSoundAudioBuffer::OnRestore()
{
	HRESULT hr;
	BOOL restored;

	// restore the buffer if it was lost
	if (FAILED(hr = RestoreBuffer(&restored)))
		return false;

	// fill the buffer with data
	if (restored)
	{
		if (FAILED(hr = FillBufferWithSound()))
			return false;
	}

	return true;
}

bool DirectSoundAudioBuffer::Play(int volume, bool looping)
{
	if (!g_pAudio->Active())
		return false;

	Stop();

	m_Volume = volume;
	m_IsLooping = looping;

	LPDIRECTSOUNDBUFFER pDSB = (LPDIRECTSOUNDBUFFER)Get();
	if (!pDSB)
		return false;

	pDSB->SetVolume(volume);

	DWORD dwFlags = looping ? DSBPLAY_LOOPING : 0L;

	return (pDSB->Play(0, 0, dwFlags) == S_OK);
}

bool DirectSoundAudioBuffer::Pause()
{
	if (!g_pAudio->Active())
		return false;

	LPDIRECTSOUNDBUFFER pDSB = (LPDIRECTSOUNDBUFFER)Get();
	if (!pDSB)
		return false;

	m_IsPaused = true;
	pDSB->Stop();
	return true;
}

bool DirectSoundAudioBuffer::Stop()
{
	if (!g_pAudio->Active())
		return false;

	LPDIRECTSOUNDBUFFER pDSB = (LPDIRECTSOUNDBUFFER)Get();
	if (!pDSB)
		return false;

	m_IsPaused = true;
	pDSB->Stop();
	// stop and rewind the sound
	pDSB->SetCurrentPosition(0);

	return true;
}

bool DirectSoundAudioBuffer::Resume()
{
	m_IsPaused = false;
	return Play(GetVolume(), IsLooping());
}

bool DirectSoundAudioBuffer::TogglePause()
{
	if (!g_pAudio->Active())
		return false;

	if (m_IsPaused)
	{
		Resume();
	}
	else
	{
		Pause();
	}

	return true;
}

bool DirectSoundAudioBuffer::IsPlaying()
{
	if (!g_pAudio->Active())
		return false;

	LPDIRECTSOUNDBUFFER pDSB = (LPDIRECTSOUNDBUFFER)Get();
	if (!pDSB)
		return false;

	DWORD dwStatus = 0;
	pDSB->GetStatus(&dwStatus);
	bool isPlaying = ((dwStatus & DSBSTATUS_PLAYING) != 0);

	return isPlaying;
}

void DirectSoundAudioBuffer::SetVolume(int volume)
{
	const int dsbVolumeMin = DSBVOLUME_MIN;

	if (!g_pAudio->Active())
		return;

	LPDIRECTSOUNDBUFFER pDSB = (LPDIRECTSOUNDBUFFER)Get();
	if (!pDSB)
		return;

	CB_ASSERT(volume >= 0 && volume <= 100 && "Volume must be a number between 0 and 100");

	// convert volume from 0 - 100 into a range DirectX can use, this uses a logarithmic scale
	float coeff = (float)volume / 100.0f;
	float logarithmicProportion = coeff > 0.1f ? 1 + log10(coeff) : 0;
	float range = (float)(DSBVOLUME_MAX - dsbVolumeMin);
	float fvolume = (range * logarithmicProportion) + dsbVolumeMin;

	CB_ASSERT(fvolume >= dsbVolumeMin && fvolume <= DSBVOLUME_MAX);
	HRESULT hr = pDSB->SetVolume((LONG)fvolume);
	CB_ASSERT(hr == S_OK);
}

void DirectSoundAudioBuffer::SetPosition(unsigned long newPosition)
{
	m_Sample->SetCurrentPosition(newPosition);
}

float DirectSoundAudioBuffer::GetProgress()
{
	LPDIRECTSOUNDBUFFER pDSB = (LPDIRECTSOUNDBUFFER)Get();
	if (!pDSB)
		return 0.0f;

	DWORD progress = 0;

	pDSB->GetCurrentPosition(&progress, nullptr);
	float length = (float)m_Resource->Size();

	return (float)progress / length;

}

HRESULT DirectSoundAudioBuffer::FillBufferWithSound()
{
	if (!m_Sample)
		return CO_E_NOTINITIALIZED;

	HRESULT hr;
	void* pDSLockedBuffer = nullptr;	// a pointer to a DirectSound buffer
	DWORD dwDSLockedBufferSize = 0;		// size of locked DirectSound buffer
	DWORD dwWavDataRead = 0;			// amount of data read from a wav file

	// make sure we have focus and didn't just switch from an app that had a DirectSound device
	if (FAILED(hr = RestoreBuffer(NULL)))
		return DXUT_ERR(L"RestoreBuffer", hr);

	int pcmBufferSize = m_Resource->Size();
	shared_ptr<SoundResourceExtraData> extra = std::static_pointer_cast<SoundResourceExtraData>(m_Resource->GetExtra());

	// lock the buffer down
	if (FAILED(hr = m_Sample->Lock(0, pcmBufferSize, &pDSLockedBuffer, &dwDSLockedBufferSize, nullptr, nullptr, 0L)));
	return DXUT_ERR(L"Locl", hr);

	if (pcmBufferSize == 0)
	{
		// wav is blank, fill with silence
		FillMemory((BYTE*)pDSLockedBuffer, dwDSLockedBufferSize, (BYTE)(extra->GetFormat()->wBitsPerSample == 8 ? 128 : 0));
	}
	else
	{
		CopyMemory(pDSLockedBuffer, m_Resource->Buffer(), pcmBufferSize);
		if (pcmBufferSize < (int)dwDSLockedBufferSize)
		{
			// if the buffer sizes are different, fill the rest in with silence
			FillMemory((BYTE*)pDSLockedBuffer + pcmBufferSize, dwDSLockedBufferSize - pcmBufferSize,
				(BYTE)(extra->GetFormat()->wBitsPerSample == 8 ? 128 : 0));
		}
	}

	// unlock the buffer
	m_Sample->Unlock(pDSLockedBuffer, dwDSLockedBufferSize, nullptr, 0);

	return S_OK;
}

HRESULT DirectSoundAudioBuffer::RestoreBuffer(BOOL* pWasRestored)
{
	if (!m_Sample)
		return CO_E_NOTINITIALIZED;

	if (pWasRestored)
		*pWasRestored = FALSE;

	HRESULT hr;
	DWORD dwStatus;

	if (FAILED(hr = m_Sample->GetStatus(&dwStatus)))
		return DXUT_ERR(L"GetStatus", hr);

	if (dwStatus & DSBSTATUS_BUFFERLOST)
	{
		// sleep until direct sound gives us control, but only for 1 second
		int count = 0;
		do
		{
			hr = m_Sample->Restore();
			if (hr = DSERR_BUFFERLOST)
				Sleep(10);
		} while (hr = m_Sample->Restore() == DSERR_BUFFERLOST && ++count < 100);

		if (pWasRestored != nullptr)
			*pWasRestored = TRUE;

		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}
