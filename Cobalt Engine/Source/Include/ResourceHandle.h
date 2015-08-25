/*
	ResourceHandle.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham.
*/

#pragma once

#include <string>

#include "interfaces.h"
#include "Resource.h"

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
	const std::string& GetName() const;

	/// Return the size of the loaded resource
	unsigned int Size() const;

	/// Return a read only pointer to the data buffer of the loaded resource
	const char* Buffer() const;

	/// Return a writable pointer to the data buffer of the loaded resource
	char* WritableBuffer();

	/// Return the extra data stored in the resource handle
	shared_ptr<IResourceExtraData> GetExtra();

	/// Set the resource handles extra data
	void SetExtra(shared_ptr<IResourceExtraData> extra);

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
