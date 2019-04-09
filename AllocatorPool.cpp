#include "AllocatorPool.h"

AllocatorPool::AllocatorPool(const size_t blkSize, const size_t numBlocks) :
	Allocator(blkSize*numBlocks), blkSize(blkSize), numBlocks(numBlocks)
{
}

void AllocatorPool::init()
{
	beginPtr = malloc(sizeTotal);
	
	void* curPtr = beginPtr;
	// initalize linked list
	for (size_t i = 0; i < numBlocks; ++i)
	{
		void* nextPtr = (char*)curPtr + blkSize;
		*((void**)curPtr) = nextPtr;
		curPtr = nextPtr;
	}

	rootFree = curPtr;
}

void AllocatorPool::free(void* ptr)
{
	
}

AllocatorPool::~AllocatorPool()
{
	free(beginPtr);
}

