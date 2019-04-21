#include "pch.h"
#include "AllocatorPoolFreeList.h"

AllocatorPoolFreeList::AllocatorPoolFreeList(const size_t blkSize, const size_t numBlocks, const size_t alignment) :
	Allocator(blkSize*numBlocks), blkSize(blkSize), numBlocks(numBlocks)
{

}

void AllocatorPoolFreeList::init()
{
	beginPtr = aligned_alloc(sizeTotal);
	reset();
}

void* AllocatorPoolFreeList::allocate(const size_t numBlocks /* = 1 */, const size_t alignment /* = 0 */)
{
	if (!rootFreePtr)
	{
		return nullptr;
	}

	if (numBlocks == 1)
	{
		void* allocatedPtr = rootFreePtr;
		rootFreePtr = (void**)(*rootFreePtr);
		return allocatedPtr;
	}
	else
	{
		assert("false" && "Allocation multiple blocks is still not implemented");
	}
}

void AllocatorPoolFreeList::free(void* ptr)
{
#if ALLOCATING_DEBUG
	assert(ptr != nullptr);
	assert(!hasOnlyDebugValue(ptr, blkSize), "Attempt to free deleted block yet");
	setDebugValue(ptr, blkSize);

	++numFreeBlocks;
#endif

	*(void**)ptr = rootFreePtr;
	rootFreePtr = (void**)ptr;
}

void AllocatorPoolFreeList::reset()
{
	void* curPtr = beginPtr;
	// initialize linked list
	for (size_t i = 0; i < numBlocks - 1; ++i)
	{
		void* nextPtr = (char*)curPtr + blkSize;
		*((void**)curPtr) = nextPtr;
		curPtr = nextPtr;
	}

	*((void**)curPtr) = nullptr; // fill the last block

	rootFreePtr = (void**)curPtr;

#if ALLOCATING_DEBUG
	setDebugValue(beginPtr, numBlocks*blkSize);
	numFreeBlocks = numBlocks;
#endif
}

AllocatorPoolFreeList::~AllocatorPoolFreeList()
{
#if ALLOCATING_DEBUG
	assert(numFreeBlocks == numBlocks && "Memory leakage!");
#endif

	free(beginPtr);
}

