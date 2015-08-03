/*
	MemoryPool.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "MemoryPool.h"

const static size_t CHUNK_HEADER_SIZE = sizeof(unsigned char*);

MemoryPool::MemoryPool()
{
	Reset();
}

MemoryPool::~MemoryPool()
{
	Destroy();
}

bool MemoryPool::Init(unsigned int chunkSize, unsigned int numChunks)
{
	// start fresh
	if (m_ppMemoryArray)
		Destroy();

	m_ChunkSize = chunkSize;
	m_NumChunks = numChunks;

	// grow the array to 1 block
	if (GrowMemoryArray())
		return true;
	return false;
}

void MemoryPool::Destroy()
{
	// call free on each block which will free each array of chunks
	for (unsigned int i = 0; i < m_MemArraySize; ++i)
	{
		free(m_ppMemoryArray[i]);
	}
	// now free the array of blocks
	free(m_ppMemoryArray);

	Reset();
}

void* MemoryPool::Alloc()
{
	// if no available memory, grow the pool
	if (!m_pHead)
	{
		// return null if growing is disabled or fails
		if (!m_AllowResize)
			return nullptr;

		if (!GrowMemoryArray())
			return nullptr;
	}

	// grab the first available chunk
	unsigned char* pRet = m_pHead;
	m_pHead = GetNext(m_pHead);

	// return a pointer to the data of the chunk, skip over the header
	return (pRet + CHUNK_HEADER_SIZE);
}

void MemoryPool::Free(void* pMem)
{
	if (pMem != nullptr)
	{
		// shift the pointer backwards to get the full chunk (header + data)
		unsigned char* pChunk = ((unsigned char*)pMem) - CHUNK_HEADER_SIZE;

		// attach this chunk to the front of the free memory list
		SetNext(pChunk, m_pHead);
		m_pHead = pChunk;
	}
}

unsigned int MemoryPool::GetChunkSize() const
{
	return m_ChunkSize;
}

void MemoryPool::SetAllowResize(bool allowResize)
{
	m_AllowResize = allowResize;
}

void MemoryPool::Reset()
{
	m_ppMemoryArray = nullptr;
	m_pHead = nullptr;
	m_ChunkSize = 0;
	m_NumChunks = 0;
	m_MemArraySize = 0;
	m_AllowResize = true;
}

bool MemoryPool::GrowMemoryArray()
{
	// new array is 1 block bigger than current memory array size
	size_t allocationSize = sizeof(unsigned char*)* (m_MemArraySize + 1);
	unsigned char** ppNewMemArray = (unsigned char**)malloc(allocationSize);

	if (!ppNewMemArray)
		return false;

	// copy over existing memory pointers
	for (unsigned int i = 0; i < m_MemArraySize; ++i)
	{
		ppNewMemArray[i] = m_ppMemoryArray[i];
	}

	// allocate new block of memory, right now it points to garbage
	// this is safe because m_memArraySize has not been incremented yet
	ppNewMemArray[m_MemArraySize] = AllocateNewMemoryBlock();

	// attach the block to the end of the current memory list
	if (m_pHead)
	{
		unsigned char* pCurr = m_pHead;
		unsigned char* pNext = GetNext(m_pHead);
		while (pNext)
		{
			pCurr = pNext;
			pNext = GetNext(pNext);
		}
		// attach the new block to the end
		SetNext(pCurr, ppNewMemArray[m_MemArraySize]);
	}
	else
	{
		m_pHead = ppNewMemArray[m_MemArraySize];
	}

	// destroy the old memory array
	if (m_ppMemoryArray)
		free(m_ppMemoryArray);

	// assign the new memory block array and increment the size
	m_ppMemoryArray = ppNewMemArray;
	++m_MemArraySize;

	return true;
}

unsigned char* MemoryPool::AllocateNewMemoryBlock()
{
	// chunkSize is a full chunk: data + header
	size_t chunkSize = m_ChunkSize + CHUNK_HEADER_SIZE;
	size_t blockSize = chunkSize * m_NumChunks;

	// allocate an entire block
	unsigned char* pNewMem = (unsigned char*)malloc(blockSize);

	if (!pNewMem)
		return nullptr;

	// turn the block of chunks into a linked list
	unsigned char* pEnd = pNewMem + blockSize; // one past the end
	unsigned char* pCurr = pNewMem;
	while (pCurr < pEnd)
	{
		// calculate the start of the next chunk
		unsigned char* pNext = pCurr + chunkSize;
		// turn the header of the current chunk in a pointer to the next chunk
		unsigned char** ppChunkHeader = (unsigned char**)pCurr;
		ppChunkHeader[0] = (pNext < pEnd ? pNext : nullptr); // point to null if at the end

		pCurr += chunkSize;
	}

	return pNewMem;
}

unsigned char* MemoryPool::GetNext(unsigned char* pChunk)
{
	// get the header of the current chunk
	unsigned char** ppChunkHeader = (unsigned char**)pChunk;
	// return where the header points (the next chunk)
	return ppChunkHeader[0];
}

void MemoryPool::SetNext(unsigned char* pChunkToChange, unsigned char* pNewNext)
{
	// get the header of the current chunk and point it to the new next
	unsigned char** ppChunkHeader = (unsigned char**)pChunkToChange;
	ppChunkHeader[0] = pNewNext;
}