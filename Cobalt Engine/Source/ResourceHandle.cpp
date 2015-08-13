/*
	ResourceHandle.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham.
*/

#include "ResourceHandle.h"

#include "EngineStd.h"
#include "ResourceCache.h"

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

const std::string& ResHandle::GetName() const
{
	return m_Resouce.m_Name;
}

unsigned int ResHandle::Size() const
{
	return m_Size;
}

const char* ResHandle::Buffer() const
{
	return m_Buffer;
}

char* ResHandle::WritableBuffer()
{
	return m_Buffer;
}

shared_ptr<IResourceExtraData> ResHandle::GetExtra()
{
	return m_Extra;
}

void ResHandle::SetExtra(shared_ptr<IResourceExtraData> extra)
{
	m_Extra = extra;
}
