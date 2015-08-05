/*
	ResourceCache.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham.

	This file contains various classes for managing resources.
*/

#pragma once

#include <functional>
#include <list>
#include <map>
#include <string>

#include "interfaces.h"
#include "ZipFile.h"

/**
	Stores the name of a resource to be used by a resource handle.
*/
class Resource
{
public:
	/// Default constructor sets the name of the resource
	Resource(const std::string& name);

	/// Name of the resource -- all lowercase
	std::string m_Name;
};

/**
	Implements the IResourceFile interface and manages a single ZipFile object
	which may contain many individual resources.
*/
class ResourceZipFile : public IResourceFile
{
public:
	/// Constructor taking a file name as a param
	ResourceZipFile(const std::wstring& resFileName);

	/// Virtual Destructor
	virtual ~ResourceZipFile();

	/// Open the file and return success or failure
	virtual bool Open();

	/// Return the size of the resource based on the name of the resource
	virtual int GetRawResourceSize(const Resource& r);

	/// Read the resource into a buffer and return how many bytes were read
	virtual int GetRawResource(const Resource& r, char* buffer);

	/// Return the number of resources in a resource file
	virtual int GetNumResources() const;

	/// Return the name of the nth resource
	virtual std::string GetResourceName(int n) const;

private:
	/// Pointer to the ZipFile object this class manages
	ZipFile *m_pZipFile;

	/// Name of the resource file on disk
	std::wstring m_resFileName;
};


/**
	This Handle pairs a loaded resource (the name) to the actual loaded data. This Handle manages 
	individual loaded resources (textures, sounds, etc.) and is responsible for the resource's raw 
	data buffer. Handles are managed by a Resource Cache.
*/
class ResHandle
{
	friend class ResCache;
public:
	/// Constructor to build a resource handle 
	ResHandle(const Resource& resource, char* buffer, unsigned int size, ResCache* pCache);
	
	/// Virtual Destructor
	virtual ~ResHandle();

	/// Return the name of the resource stored in the handle
	inline const std::string& GetName() const;

	/// Return the size of the loaded resource
	inline unsigned int Size() const;

	/// Return a read only pointer to the data buffer of the loaded resource
	inline const char* Buffer() const;

	/// Return a writable pointer to the data buffer of the loaded resource
	inline char* WritableBuffer();

	/// Return the extra data stored in the resource handle
	inline shared_ptr<IResourceExtraData> GetExtra();

	/// Set the resource handles extra data
	inline void SetExtra(shared_ptr<IResourceExtraData> extra);

protected:
	/// The resource that is loaded
	Resource m_Resouce;

	/// Pointer to the raw loaded data
	char* m_Buffer;

	/// Size of the loaded resource
	unsigned int m_Size;

	/// Extra data belonging to the resource
	shared_ptr<IResourceExtraData> m_Extra;
	
	/// Pointer to the resource cache that owns this resource handle
	ResCache* m_pResCache;
};

typedef std::list<shared_ptr<ResHandle>> ResHandleList;
typedef std::map<std::string, shared_ptr<ResHandle>> ResHandleMap;
typedef std::list<shared_ptr<IResourceLoader>> ResourceLoaders;
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


/**
	Used to load resources that need no additional processing on load. It will use the 
	raw data that has already been loaded into memory from the IResourceFile and is now
	stored in the ResHandle.
*/
class DefaultResourceLoader : public IResourceLoader
{
public:
	/// Return * as the default pattern
	virtual std::string GetPattern() { return "*"; }

	/// Return true, use the raw file without additional processing
	virtual bool UseRawFile() { return true; }

	/// Return the raw size
	virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) { return rawSize; }

	/// Return true, the resource needs no additional logic to load
	virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle) { return true; }
};