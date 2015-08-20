/*
	DirectSoundAudio.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "DirectSoundAudio.h"

#include <winerror.h>

#include "DirectSoundAudioBuffer.h"
#include "EngineStd.h"
#include "Logger.h"
#include "SoundResourceExtraData.h"

DirectSoundAudio::DirectSoundAudio()
{
	m_pDS = nullptr;
}

bool DirectSoundAudio::Initialize(HWND hWnd)
{
	if (m_Initialized)
		return true;
	
	m_Initialized = false;
	m_AllSamples.clear();

	CB_COM_RELEASE(m_pDS);

	HRESULT hr;

	/// create direct sound 8
	if (FAILED(hr = DirectSoundCreate8(NULL, &m_pDS, NULL)))
		return false;

	// set coop level
	if (FAILED(hr = m_pDS->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)))
		return false;

	// set primary audio buffer format
	if (FAILED(hr = SetPrimaryBufferFormat(8, 4410, 16)))
		return false;

	m_Initialized = true;

	return true;
}

void DirectSoundAudio::Shutdown()
{
	if (m_Initialized)
	{
		Audio::Shutdown();
		CB_COM_RELEASE(m_pDS);
		m_Initialized = false;
	}
}

bool DirectSoundAudio::Active()
{
	return m_pDS != nullptr;
}

IAudioBuffer* DirectSoundAudio::InitAudioBuffer(shared_ptr<ResHandle> soundResource)
{
	shared_ptr<SoundResourceExtraData> extra = std::static_pointer_cast<SoundResourceExtraData>(soundResource->GetExtra());

	if (!m_pDS)
		return nullptr;

	switch (extra->GetSoundType())
	{
	case SoundType::SOUND_TYPE_OGG:
	case SoundType::SOUND_TYPE_WAVE:
		// break because we support ogg and wave
		break;
		
	case SoundType::SOUND_TYPE_MP3:
	case SoundType::SOUND_TYPE_MIDI:
		CB_ASSERT(false && "MP3's and MIDI are not supported");
		return nullptr;

	default:
		CB_ASSERT(false && "Unknown sound type");
		return nullptr;
	}

	LPDIRECTSOUNDBUFFER sampleHandle;

	// create the direct sound buffer
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRLVOLUME;
	dsbd.dwBufferBytes = soundResource->Size();
	dsbd.guid3DAlgorithm = GUID_NULL;
	dsbd.lpwfxFormat = const_cast<WAVEFORMATEX*>(extra->GetFormat());

	HRESULT hr;
	if (FAILED(hr = m_pDS->CreateSoundBuffer(&dsbd, &sampleHandle, nullptr)))
		return nullptr;

	// create a direct sound audio buffer and push it to the list of samples
	IAudioBuffer* audioBuffer = CB_NEW DirectSoundAudioBuffer(sampleHandle, soundResource);
	m_AllSamples.push_front(audioBuffer);

	return audioBuffer;
}

void DirectSoundAudio::ReleaseAudioBuffer(IAudioBuffer* audioBuffer)
{
	audioBuffer->Stop();
	m_AllSamples.remove(audioBuffer);
}

HRESULT DirectSoundAudio::SetPrimaryBufferFormat(DWORD primaryChannels, DWORD primaryFreq, DWORD primaryBitRate)
{
	HRESULT hr;
	LPDIRECTSOUNDBUFFER pDSBuffer = nullptr;

	if (!m_pDS)
		return CO_E_NOTINITIALIZED;

	// create the primary sound buffer
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat = nullptr;

	if (FAILED(hr = m_pDS->CreateSoundBuffer(&dsbd, &pDSBuffer, nullptr)))
		return DXUT_ERR(L"CreateSoundBuffer", hr);

	// set the sound format
	WAVEFORMATEX wfx;
	ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
	wfx.wFormatTag = (WORD)WAVE_FORMAT_PCM;
	wfx.nChannels = (WORD)primaryChannels;
	wfx.nSamplesPerSec = (DWORD)primaryFreq;
	wfx.wBitsPerSample = (WORD)primaryBitRate;
	wfx.nBlockAlign = (WORD)(wfx.wBitsPerSample / 8 * wfx.nChannels);
	wfx.nAvgBytesPerSec = (DWORD)(wfx.nSamplesPerSec * wfx.nBlockAlign);

	if (FAILED(hr = pDSBuffer->SetFormat(&wfx)))
		return DXUT_ERR(L"SetFormat", hr);

	CB_COM_RELEASE(pDSBuffer);

	return S_OK;
}
