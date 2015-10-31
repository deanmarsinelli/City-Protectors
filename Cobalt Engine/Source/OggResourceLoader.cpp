/*
	OggResourceLoader.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <codec.h>
#include <vorbisfile.h>

#include "EngineStd.h"
#include "Logger.h"
#include "OggResourceLoader.h"
#include "SoundResourceExtraData.h"


#ifdef _DEBUG
	#pragma comment(lib, "libogg_staticd.lib")
	#pragma comment(lib, "libvorbis_staticd.lib")
	#pragma comment(lib, "libvorbisfile_staticd.lib")
#else
	#pragma comment(lib, "libogg_static.lib")
	#pragma comment(lib, "libvorbis_static.lib")
	#pragma comment(lib, "libvorbisfile_static.lib")
#endif


// represents an ogg file in memory
struct OggMemoryFile
{
	// pointer to the data in memory
	unsigned char* dataPtr;

	// size of the data
	size_t dataSize;

	// bytes read so far
	size_t dataRead;

	OggMemoryFile()
	{
		dataPtr = nullptr;
		dataSize = 0;
		dataRead = 0;
	}
};


// C callback functions for the vorbis library
size_t VorbisRead(void* data_ptr, size_t byteSize, size_t sizeToRead, void* data_src)
{
	OggMemoryFile *pVorbisData = static_cast<OggMemoryFile *>(data_src);
	if (NULL == pVorbisData)
	{
		return -1;
	}

	size_t actualSizeToRead, spaceToEOF =
		pVorbisData->dataSize - pVorbisData->dataRead;
	if ((sizeToRead*byteSize) < spaceToEOF)
	{
		actualSizeToRead = (sizeToRead*byteSize);
	}
	else
	{
		actualSizeToRead = spaceToEOF;
	}

	if (actualSizeToRead)
	{
		memcpy(data_ptr,
			(char*)pVorbisData->dataPtr + pVorbisData->dataRead, actualSizeToRead);
		pVorbisData->dataRead += actualSizeToRead;
	}

	return actualSizeToRead;
}

int VorbisSeek(void* data_src, ogg_int64_t offset, int origin)
{
	OggMemoryFile *pVorbisData = static_cast<OggMemoryFile *>(data_src);
	if (NULL == pVorbisData)
	{
		return -1;
	}

	switch (origin)
	{
	case SEEK_SET:
	{
		ogg_int64_t actualOffset;
		actualOffset = (pVorbisData->dataSize >= offset) ? offset : pVorbisData->dataSize;
		pVorbisData->dataRead = static_cast<size_t>(actualOffset);
		break;
	}

	case SEEK_CUR:
	{
		size_t spaceToEOF = pVorbisData->dataSize - pVorbisData->dataRead;

		ogg_int64_t actualOffset;
		actualOffset = (offset < spaceToEOF) ? offset : spaceToEOF;

		pVorbisData->dataRead += static_cast<LONG>(actualOffset);
		break;
	}

	case SEEK_END:
		pVorbisData->dataRead = pVorbisData->dataSize + 1;
		break;

	default:
		CB_ASSERT(0 && "Bad parameter for 'origin', requires same as fseek.");
		break;
	};

	return 0;
}

int VorbisClose(void *src)
{
	return 0;
}

long VorbisTell(void *data_src)
{
	OggMemoryFile *pVorbisData = static_cast<OggMemoryFile *>(data_src);
	if (NULL == pVorbisData)
	{
		return -1L;
	}

	return static_cast<long>(pVorbisData->dataRead);
}


shared_ptr<IResourceLoader> CreateOggResourceLoader()
{
	return shared_ptr<IResourceLoader>(CB_NEW OggResourceLoader());
}

std::string OggResourceLoader::GetPattern()
{
	return "*.ogg";
}

bool OggResourceLoader::UseRawFile()
{
	return false;
}

bool OggResourceLoader::DiscardRawBufferAfterLoad()
{
	return true;
}

unsigned int OggResourceLoader::GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize)
{
	OggVorbis_File vf;

	ov_callbacks oggCallbacks;

	// create the vorbis memory object
	OggMemoryFile* vorbisMemoryFile = CB_NEW OggMemoryFile;
	vorbisMemoryFile->dataSize = rawSize;
	vorbisMemoryFile->dataPtr = (unsigned char*)rawBuffer;
	vorbisMemoryFile->dataRead = 0;

	// set up ogg callbacks
	oggCallbacks.read_func = VorbisRead;
	oggCallbacks.close_func = VorbisClose;
	oggCallbacks.seek_func = VorbisSeek;
	oggCallbacks.tell_func = VorbisTell;

	int ov_ret = ov_open_callbacks(vorbisMemoryFile, &vf, nullptr, 0, oggCallbacks);
	CB_ASSERT(ov_ret >= 0);

	
	vorbis_info* vi = ov_info(&vf, -1);

	DWORD size = 4096 * 16;
	DWORD pos = 0;
	int sec = 0;
	int ret = 1;

	DWORD bytes = (DWORD)ov_pcm_total(&vf, -1);
	bytes *= 2 * vi->channels;

	ov_clear(&vf);

	CB_SAFE_DELETE(vorbisMemoryFile);

	return bytes;
}

bool OggResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	// create the resources extra data
	shared_ptr<SoundResourceExtraData> extra = shared_ptr<SoundResourceExtraData>(CB_NEW SoundResourceExtraData);
	extra->m_SoundType = SoundType::SOUND_TYPE_OGG;
	handle->SetExtra(extra);

	// load the ogg into the handle
	if (!ParseOgg(rawBuffer, rawSize, handle))
	{
		return false;
	}

	return true;
}

bool OggResourceLoader::ParseOgg(char* oggStream, size_t bufferLength, shared_ptr<ResHandle> handle)
{
	shared_ptr<SoundResourceExtraData> extra = static_pointer_cast<SoundResourceExtraData>(handle->GetExtra());

	OggVorbis_File vf;

	ov_callbacks oggCallbacks;

	// create the vorbis memory object
	OggMemoryFile* vorbisMemoryFile = CB_NEW OggMemoryFile;
	vorbisMemoryFile->dataSize = bufferLength;
	vorbisMemoryFile->dataPtr = (unsigned char*)oggStream;
	vorbisMemoryFile->dataRead = 0;

	// set up ogg callbacks
	oggCallbacks.read_func = VorbisRead;
	oggCallbacks.close_func = VorbisClose;
	oggCallbacks.seek_func = VorbisSeek;
	oggCallbacks.tell_func = VorbisTell;

	int ov_ret = ov_open_callbacks(vorbisMemoryFile, &vf, nullptr, 0, oggCallbacks);
	CB_ASSERT(ov_ret >= 0);

	vorbis_info* vi = ov_info(&vf, -1);

	ZeroMemory(&(extra->m_WavFormatEx), sizeof(extra->m_WavFormatEx));

	// set up the extra info
	extra->m_WavFormatEx.cbSize = sizeof(extra->m_WavFormatEx);
	extra->m_WavFormatEx.nChannels = vi->channels;
	extra->m_WavFormatEx.wBitsPerSample = 16;
	extra->m_WavFormatEx.nSamplesPerSec = vi->rate;
	extra->m_WavFormatEx.nAvgBytesPerSec = extra->m_WavFormatEx.nSamplesPerSec * extra->m_WavFormatEx.nChannels * 2;
	extra->m_WavFormatEx.nBlockAlign = extra->m_WavFormatEx.nChannels * 2;
	extra->m_WavFormatEx.wFormatTag = 1;

	DWORD size = 4096 * 16;
	DWORD pos = 0;
	int sec = 0;
	int ret = 1;

	DWORD bytes = (DWORD)ov_pcm_total(&vf, -1);
	bytes *= 2 * vi->channels;

	if (handle->Size() != bytes)
	{
		CB_ASSERT(0 && L"The Ogg size does not match the memory buffer size");
		ov_clear(&vf);
		CB_SAFE_DELETE(vorbisMemoryFile);
		return false;
	}

	// read in the bytes
	while (ret && pos < bytes)
	{
		ret = ov_read(&vf, handle->WritableBuffer() + pos, size, 0, 2, 1, &sec);
		pos += ret;

		if (bytes - pos < size)
		{
			size = bytes - pos;
		}
	}

	extra->m_LengthMilliseconds = (int)(1000.0f * ov_time_total(&vf, -1));

	ov_clear(&vf);
	CB_SAFE_DELETE(vorbisMemoryFile);
	
	return true;
}
