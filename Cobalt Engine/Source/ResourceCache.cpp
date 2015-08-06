/*
	ResourceCache.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham.
*/

#include "ResourceCache.h"

#include <algorithm>
#include <cctype>

#include "DefaultResourceLoader.h"
#include "EngineStd.h"
#include "Logger.h"
#include "StringUtil.h"

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
		if (cancel)
			break;

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

void ResCache::Flush()
{
	while (!m_LRU.empty())
	{
		shared_ptr<ResHandle> handle = m_LRU.front();
		Free(handle);
		m_LRU.pop_front();
	}
}

shared_ptr<ResHandle> ResCache::Find(Resource* r)
{
	ResHandleMap::iterator it = m_Resources.find(r->m_Name);
	if (it == m_Resources.end())
		return nullptr;

	return it->second;
}

const void* ResCache::Update(shared_ptr<ResHandle> handle)
{
	m_LRU.remove(handle);
	m_LRU.push_front(handle);
}

shared_ptr<ResHandle> ResCache::Load(Resource* r)
{
	shared_ptr<IResourceLoader> loader;
	shared_ptr<ResHandle> handle;

	// find the correct loader to load this resource
	for (ResourceLoaders::iterator it = m_ResourceLoaders.begin(); it != m_ResourceLoaders.end(); ++it)
	{
		shared_ptr<IResourceLoader> testLoader = *it;

		if (WildcardMatch(testLoader->GetPattern().c_str(), r->m_Name.c_str()))
		{
			loader = testLoader;
			break;
		}
	}
	
	if (!loader)
	{
		CB_ASSERT(loader && L"Could not find a resource loader");
		return nullptr;
	}

	// find the resource in the file
	int rawSize = m_File->GetRawResourceSize(*r);
	if (rawSize < 0)
	{
		CB_ASSERT(rawSize > 0 && "Resource not found");
		return nullptr;
	}

	// allocate a buffer to hold the resource in memory
	int allocSize = rawSize + ((loader->AddNullZero()) ? (1) : (0));
	// if not using the raw file, the raw buffer is allocated outside and is temporary
	char* rawBuffer = loader->UseRawFile() ? Allocate(allocSize) : CB_NEW char[allocSize];
	ZeroMemory(rawBuffer, allocSize);

	// load the resource from disk into the memory buffer
	if (rawBuffer == nullptr || m_File->GetRawResource(*r, rawBuffer) == 0)
	{
		CB_LOG("Out of Memory");
		return nullptr;
	}

	char* buffer = nullptr;
	unsigned int size = 0;

	// if the loader uses raw files, create a handle for the resource using the raw buffer
	if (loader->UseRawFile())
	{
		buffer = rawBuffer;
		handle = shared_ptr<ResHandle>(CB_NEW ResHandle(*r, buffer, rawSize, this));
	}
	else
	{
		// if the file requires more processing, get its loaded size, load it into a buffer
		// of that size, and then load the resource appropriately
		size = loader->GetLoadedResourceSize(rawBuffer, rawSize);
		// allocate a new buffer for the loaded resource
		buffer = Allocate(size);
		if (buffer == nullptr)
		{
			CB_LOG("Out of Memory");
			return nullptr;
		}
		handle = shared_ptr<ResHandle>(CB_NEW ResHandle(*r, buffer, size, this));
		bool success = loader->LoadResource(rawBuffer, rawSize, handle);

		// delete the temporary raw buffer after the loaded resource is created
		if (loader->DiscardRawBufferAfterLoad())
		{
			CB_SAFE_DELETE_ARRAY(rawBuffer);
		}

		if (!success)
		{
			CB_LOG("Coule not load resource");
			return nullptr;
		}
	}

	if (handle)
	{
		// if a handle was successfully created, add it to the list and map
		m_LRU.push_front(handle);
		m_Resources[r->m_Name] = handle;
	}

	return handle;
}

void ResCache::Free(shared_ptr<ResHandle> handle)
{
	// removes the item from the cache, but the item might still be in memory
	// if a shared_ptr still exists somewhere
	m_LRU.remove(handle);
	m_Resources.erase(handle->m_Resouce.m_Name);
}

bool ResCache::MakeRoom(unsigned int size)
{
	// if trying to make more room than the total cache, return false
	if (size > m_CacheSize)
	{
		return false;
	}

	// while the size needed is still larger than the free space, keep removing resources
	while (size > (m_CacheSize - m_Allocated))
	{
		// if the cache is empty and theres still not enough room, return false
		if (m_LRU.empty())
		{
			return false;
		}

		FreeOneResource();
	}

	return true;
}

char* ResCache::Allocate(unsigned int size)
{
	/// if the cache cannot create enough room, return null
	if (!MakeRoom(size))
	{
		return nullptr;
	}
	
	// allocate the memory and return a pointer to it
	char* memory = CB_NEW char[size];
	if (memory)
	{
		m_Allocated += size;
	}
	
	return memory;
}

void ResCache::FreeOneResource()
{
	// remove the resource at the back of the LRU list -- the object may still
	// exist in memory if a shared_ptr is held onto it outside of the cache
	shared_ptr<ResHandle> handleToBeRemoved = m_LRU.back();

	m_LRU.pop_back();
	m_Resources.erase(handleToBeRemoved->m_Resouce.m_Name);
}

void ResCache::MemoryHasBeenFreed(unsigned int size)
{
	m_Allocated -= size;
}