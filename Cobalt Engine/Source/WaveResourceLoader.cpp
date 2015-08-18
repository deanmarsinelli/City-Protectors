/*
	WaveResourceLoader.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "WaveResourceLoader.h"

#include <mmsystem.h>

#include "Logger.h"
#include "SoundResourceExtraData.h"

std::string WaveResourceLoader::GetPattern()
{
	return "*.wav";
}

bool WaveResourceLoader::UseRawFile()
{
	return false;
}

bool WaveResourceLoader::DiscardRawBufferAfterLoad()
{
	return true;
}

unsigned int WaveResourceLoader::GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize)
{
	DWORD file = 0;
	DWORD fileEnd = 0;
	DWORD length = 0;
	DWORD type = 0;
	DWORD pos = 0;

	// the first 4 bytes of a valid .wav file is 'R','I','F','F'
	type = *((DWORD*)(rawBuffer + pos));
	pos += sizeof(DWORD);

	// mmioFOURCC converts 4 chars into a 4 byte integer code (DWORD)
	if (type != mmioFOURCC('R', 'I', 'F', 'F'))
	{
		return false;
	}

	// next 4 bytes are the length
	length = *((DWORD*)(rawBuffer + pos));
	pos += sizeof(DWORD);

	// next 4 bytes are the type
	type = *((DWORD*)(rawBuffer + pos));
	pos += sizeof(DWORD);
	if (type != mmioFOURCC('W', 'A', 'V', 'E'))
	{
		return false;
	}

	fileEnd = length - 4;

	// traverse the data chunks looking for 'data'
	while (file < fileEnd)
	{
		type = *((DWORD*)(rawBuffer + pos));
		pos += sizeof(DWORD);
		file += sizeof(DWORD);

		length = *((DWORD*)(rawBuffer + pos));
		pos += sizeof(DWORD);
		file += sizeof(DWORD);

		switch (type)
		{
		case mmioFOURCC('f', 'a', 'c', 't'):
			CB_ASSERT(false && "This wave is compressed and we cannot handle it");
			break;

		case mmioFOURCC('f', 'm', 't', ' '):
			pos += length;
			break;

		case mmioFOURCC('d', 'a', 't', 'a'):
			return length;
		}

		file += length;

		// increment the pointer past the block we just read and make sure its word aligned
		if (length & 1)
		{
			++pos;
			++file;
		}
	}

	// return false if the wave did not have the correct data
	return false;
}

bool WaveResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	// attach extra data onto the resource handle marking this as a wave file
	shared_ptr<SoundResourceExtraData> extra = shared_ptr<SoundResourceExtraData>(CB_NEW SoundResourceExtraData);
	extra->m_SoundType = SoundType::SOUND_TYPE_WAVE;
	handle->SetExtra(shared_ptr<SoundResourceExtraData>(extra));

	// parse the wave to load it correctly
	if (!ParseWave(rawBuffer, rawSize, handle))
	{
		return false;
	}

	return true;
}

bool WaveResourceLoader::ParseWave(char* wavStream, size_t bufferLength, shared_ptr<ResHandle> handle)
{
	shared_ptr<SoundResourceExtraData> extra = std::static_pointer_cast<SoundResourceExtraData>(handle->GetExtra());
	
	DWORD file = 0;
	DWORD fileEnd = 0;
	DWORD length = 0;
	DWORD type = 0;
	DWORD pos = 0;

	// the first 4 bytes of a valid .wav file is 'R','I','F','F'
	type = *((DWORD*)(wavStream + pos));
	pos += sizeof(DWORD);

	// mmioFOURCC converts 4 chars into a 4 byte integer code (DWORD)
	if (type != mmioFOURCC('R', 'I', 'F', 'F'))
	{
		return false;
	}

	// next 4 bytes are the length
	length = *((DWORD*)(wavStream + pos));
	pos += sizeof(DWORD);

	// next 4 bytes are the type
	type = *((DWORD*)(wavStream + pos));
	pos += sizeof(DWORD);
	if (type != mmioFOURCC('W', 'A', 'V', 'E'))
	{
		// not a wave file
		return false;
	}

	fileEnd = length - 4;
	ZeroMemory(&extra->m_WavFormatEx, sizeof(WAVEFORMATEX));

	bool copiedBuffer = false;

	// load the .wav format and the .wav data, these can be in either order
	while (file < fileEnd)
	{
		type = *((DWORD*)(wavStream + pos));
		pos += sizeof(DWORD);
		file += sizeof(DWORD);

		length = *((DWORD*)(wavStream + pos));
		pos += sizeof(DWORD);

		switch (type)
		{
		case mmioFOURCC('f', 'a', 'c', 't'):
			CB_ASSERT(false && "This wave is compressed and we cannot handle it");
			break;

		case mmioFOURCC('f', 'm', 't', ' '):
			memcpy(&extra->m_WavFormatEx, wavStream + pos, length);
			pos += length;
			extra->m_WavFormatEx.cbSize = (WORD)length;
			break;

		case mmioFOURCC('d', 'a', 't', 'a'):  // the actual sound data
			copiedBuffer = true;
			if (length != handle->Size())
			{
				CB_ASSERT(0 && L"Wav resource size does not equal the buffer size");
				return false;
			}
			memcpy(handle->WritableBuffer(), wavStream + pos, length);
			pos += length;
			break;
		}

		file += length;

		// if the buffer was copied, fill in the length and return true
		if (copiedBuffer)
		{
			extra->m_LengthMilliseconds = (handle->Size() * 1000) / extra->GetFormat()->nAvgBytesPerSec;
			return true;
		}

		// increment the pointer past the block we just read and make sure its word aligned
		if (length & 1)
		{
			++pos;
			++file;
		}
	}

	// return false if the wav file did not have the right data
	return false;
}
