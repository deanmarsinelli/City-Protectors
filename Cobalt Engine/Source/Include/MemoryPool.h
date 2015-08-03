/*
	MemoryPool.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <cstdlib>

/**
	Memory in this MemoryPool class is allocated in an
	array of blocks, with each block pointing to an array of
	chunks.

	Chunks are equal sized and contain the actual objects plus a
	4 - byte header which points to the next chunk.They are linked
	in a singly linked list with m_pHead pointing to the first
	unused chunk of free memory.Chunk size and number of chunks
	per block are written at Init() time and cannot be changed
	without reinitializing the pool.

	Chunks are obtained by calling Alloc() which returns the
	first free chunk of memory in the list.If no memory is
	available, Alloc() will attempt to allocate a new block
	which can have a high performance cost.

	Free() will release a chunk of memory and insert it into
	the front of the list of available memory.

	[raw] ->[block][block]
				|	 |
	[head] ->[chunk][chunk]
	   		 [chunk][chunk]
			 [chunk][chunk]
*/
class MemoryPool
{
public:
	// constructor and destructor
	MemoryPool();
	~MemoryPool();

	// initialize the memory pool
	bool Init(unsigned int chunkSize, unsigned int numChunks);
	// free the entire pool
	void Destroy();

	// allocate and free chunks of memory
	void* Alloc();
	void Free(void* pMem);
	unsigned int GetChunkSize() const;

	// enable/disable the pool to allocate more memory when full
	void SetAllowResize(bool allowResize);

private:
	// resets the internal variables
	void Reset();

	// memory allocation helpers
	bool GrowMemoryArray();
	unsigned char* AllocateNewMemoryBlock();

	// linked list management
	unsigned char* GetNext(unsigned char* pChunk);
	void SetNext(unsigned char* pChunkToChange, unsigned char* pNewNext);

	// no copying allowed!
	MemoryPool(const MemoryPool& /*memPool*/) { }

private:
	// array of memory blocks, split into chunks
	unsigned char** m_ppMemoryArray;

	// the front of the memory chunk linked list (first availble chunk)
	unsigned char* m_pHead;

	// size of each chunk and number of chunks per array
	unsigned int m_ChunkSize, m_NumChunks;

	// number of elements in the memory array
	unsigned int m_MemArraySize;

	// true if we resize the mrmoy pool when it fills
	bool m_AllowResize;
};