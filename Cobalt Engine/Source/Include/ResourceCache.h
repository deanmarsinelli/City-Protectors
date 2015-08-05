/*
	ResourceCache.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham.
*/

#pragma once

#include <functional>
#include <list>
#include <map>
#include <string>

#include "interfaces.h"
#include "ResourceHandle.h"


/**
	Caches resources (as ResHandle's) that are currently loaded into memory in an LRU fashion. 
	This Resource Cache stores two pointers to every currently loaded ResHandle. 
	
	The first is in a linked list where the nodes appear in order in which the resource was 
	last used. When a resource is used, it is moved to the front of the list.  The front of 
	the list is most recently used resources and the end of the list is least recently used.

	The second is stored in a map to quickly find resource data with the unique resource id.

	A list of resource loaders for all the different types stored in this cache is also kept.
*/
class ResCache
{
	friend class ResHandle;
	typedef std::list<shared_ptr<ResHandle>> ResHandleList;
	typedef std::map<std::string, shared_ptr<ResHandle>> ResHandleMap;
	typedef std::list<shared_ptr<IResourceLoader>> ResourceLoaders;
public:
	/// Construct the cache with a max size and resource file
	ResCache(const unsigned int sizeInMb, IResourceFile *resourceFile);

	/// Default Destructor
	~ResCache();

	/// Initialize the cache
	bool Init();

	/// Register a resource loader with this cache
	void RegisterLoader(shared_ptr<IResourceLoader> loader);

	/// Return a handle given a particular resource
	shared_ptr<ResHandle> GetHandle(Resource* r);

	int PreLoad(const std::string& pattern, std::function<void(int, bool&)> progressCallback);

	/// Flush the cache removing everything from memory
	void Flush();

protected:
	// TODO: document these
	shared_ptr<ResHandle> Find(Resource* r);
	const void* Update(shared_ptr<ResHandle> handle);
	shared_ptr<ResHandle> Load(Resource* r);
	void Free(shared_ptr<ResHandle> handle);
	bool MakeRoom(unsigned int size);
	char* Allocated(unsigned int size);
	void FreeOneResource();
	void MemoryHasBeenFreed(unsigned int size);

protected:
	/// The LRU cache holding pointers to resource handles
	ResHandleList m_LRU;

	/// A map of names to resource handles
	ResHandleMap m_Resources;

	/// A list of loaders for the files in this cache
	ResourceLoaders m_ResourceLoaders;

	/// A resource file that can be used by a loader to load a resource handle into the cache
	IResourceFile* m_File;

	/// Total size of the cache
	unsigned int m_CacheSize;

	/// Total memory currently allocated
	unsigned int m_Allocated;
};