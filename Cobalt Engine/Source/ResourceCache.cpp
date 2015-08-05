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