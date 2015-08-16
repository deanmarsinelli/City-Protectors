/*
	ResourceCache.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham.
*/

#pragma once

#include <functional>
#include <list>
#include <string>
#include <unordered_map>

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
	typedef std::unordered_map<std::string, shared_ptr<ResHandle>> ResHandleMap;
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

	/// Return a handle given a particular resource. If it is not yet in the cache it will be loaded
	shared_ptr<ResHandle> GetHandle(Resource* r);

	/// Preload resources matching the pattern into the cache
	int PreLoad(const std::string& pattern, std::function<void(int, bool&)> progressCallback);

	/// Flush the cache removing everything from memory
	void Flush();

	/// Return true if using the games development directories
	bool IsUsingDevelopmentDirectories() const;

protected:
	/// Return a handle to a resource if it exists in the cache
	shared_ptr<ResHandle> Find(Resource* r);

	/// Push the handle to the front of the LRU list marking it as most recently used
	const void* Update(shared_ptr<ResHandle> handle);

	/// Load a resource from disk into the resource cache
	shared_ptr<ResHandle> Load(Resource* r);

	/// Remove an item from cache -- memory will not be freed until ref count of the object is 0
	void Free(shared_ptr<ResHandle> handle);

	/// Attempt to make room in the cache for a given size
	bool MakeRoom(unsigned int size);

	/// Allocate space for an object and return a pointer to that memory
	char* Allocate(unsigned int size);

	/// Remove the least recently used item from the cache -- memory will not be freed until ref count of the object is 0
	void FreeOneResource();

	/// Decrease the total amount of allocated memory -- call this when the handle is finally freed
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