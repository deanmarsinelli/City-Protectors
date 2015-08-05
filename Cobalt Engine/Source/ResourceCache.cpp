/*
	ResourceCache.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham.
*/

#include "ResourceCache.h"

#include <algorithm>
#include <cctype>

#include "EngineStd.h"
#include "Logger.h"

// --------------------------------------------
//  Resource methods
// --------------------------------------------
Resource::Resource(const std::string& name)
{
	m_Name = name;
	std::transform(m_Name.begin(), m_Name.end(), m_Name.begin(), (int(*)(int)) std::tolower);
}

// --------------------------------------------
//  ResourceZipFile methods
// --------------------------------------------
ResourceZipFile::ResourceZipFile(const std::wstring& resFileName) :
m_pZipFile(nullptr),
m_resFileName(resFileName)
{}

ResourceZipFile::~ResourceZipFile()
{
	CB_SAFE_DELETE(m_pZipFile);
}

bool ResourceZipFile::Open()
{
	m_pZipFile = CB_NEW ZipFile;
	if (m_pZipFile)
	{
		return m_pZipFile->Init(m_resFileName);
	}
	return false;
}

int ResourceZipFile::GetRawResourceSize(const Resource& r)
{
	int resourceNum = m_pZipFile->Find(r.m_Name);
	if (resourceNum == -1)
		return -1;
	
	return m_pZipFile->GetFileLength(resourceNum);
}

int ResourceZipFile::GetRawResource(const Resource& r, char* buffer)
{
	int size = 0;
	int resourceNum = m_pZipFile->Find(r.m_Name);
	if (resourceNum >= 0)
	{
		size = m_pZipFile->GetFileLength(resourceNum);
		m_pZipFile->ReadFile(resourceNum, buffer);
	}

	return size;
}

int ResourceZipFile::GetNumResources() const
{
	return (m_pZipFile == nullptr) ? 0 : m_pZipFile->GetNumFiles();
}

std::string ResourceZipFile::GetResourceName(int n) const
{
	std::string resourceName = "";
	if (m_pZipFile != nullptr && n >= 0 && n < m_pZipFile->GetNumFiles())
	{
		resourceName = m_pZipFile->GetFileName(n);
	}

	return resourceName;
}

// --------------------------------------------
//  ResHandle methods
// --------------------------------------------
ResHandle::ResHandle(const Resource& resource, char* buffer, unsigned int size, ResCache* pCache) :
m_Resouce(resource),
m_Buffer(buffer),
m_Size(size),
m_Extra(nullptr),
m_pResCache(pCache)
{}

ResHandle::~ResHandle()
{
	CB_SAFE_DELETE(m_Buffer);
	// tell the resource cache how much memory has been freed
	m_pResCache->MemoryHasBeenFreed(m_Size);
}

inline const std::string& ResHandle::GetName() const
{
	return m_Resouce.m_Name;
}

inline unsigned int ResHandle::Size() const
{
	return m_Size;
}

inline const char* ResHandle::Buffer() const
{
	return m_Buffer;
}

inline char* ResHandle::WritableBuffer()
{
	return m_Buffer;
}

inline shared_ptr<IResourceExtraData> ResHandle::GetExtra()
{
	return m_Extra;
}

inline void ResHandle::SetExtra(shared_ptr<IResourceExtraData> extra)
{
	m_Extra = extra;
}

// --------------------------------------------
//  ResCache methods
// --------------------------------------------
ResCache::ResCache(const unsigned int sizeInMb, IResourceFile* resourceFile)
{
	m_CacheSize = sizeInMb * 1024 * 1024;
	m_Allocated = 0;
	m_File = resourceFile;
}

ResCache::~ResCache()
{
	while (!m_LRU.empty())
	{
		FreeOneResource();
	}
	CB_SAFE_DELETE(m_File);
}

bool ResCache::Init()
{
	bool val = false;
	// open the resource file and register a default loader
	if (m_File->Open())
	{
		RegisterLoader(shared_ptr<IResourceLoader>(CB_NEW DefaultResourceLoader));
		val = true;
	}
	return val;
}

void ResCache::RegisterLoader(shared_ptr<IResourceLoader> loader)
{
	// the most generic loader is last in the list, so other loaders
	// get a shot loading files before it
	m_ResourceLoaders.push_front(loader);
}

shared_ptr<ResHandle> ResCache::GetHandle(Resource* r)
{
	// attempt to locate the handle in the cache
	shared_ptr<ResHandle> handle(Find(r));
	if (handle == nullptr)
	{
		// if the handle is not in the cache (miss), load it
		handle = Load(r);
		CB_ASSERT(handle);
	}
	else
	{
		// if the handle was in the cache (hit), move it to the front
		// of the LRU list
		Update(handle);
	}

	return handle;
}

int ResCache::PreLoad(const std::string& pattern, std::function<void(int, bool&)> progressCallback)
{
	if (m_File == nullptr)
		return 0;

	int numFiles = m_File->GetNumResources();
	int loaded = 0;
	bool cancel = false;

	for (int i = 0; i < numFiles; ++i)
	{
		Resource resource(m_File->GetResourceName(i));

		if (WildcardMatch(pattern.c_str(), resource.m_Name.c_str()))
		{
			shared_ptr<ResHandle> handle = g_pApp->m_ResCache->GetHandle(&resource);
			++loaded;
		}

		if (progressCallback != nullptr)
		{
			progressCallback(i * 100 / numFiles, cancel);
		}
	}
	return loaded;
}