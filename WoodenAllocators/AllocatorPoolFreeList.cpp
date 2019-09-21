#include "pch.h"
#include "AllocatorPoolFreeList.h"

namespace wal{

AllocatorPoolFreeList::AllocatorPoolFreeList()
{}

AllocatorPoolFreeList::AllocatorPoolFreeList(size_t blkSize, size_t numBlocks, size_t alignment) :
	Allocator(blkSize*numBlocks), blkSize(blkSize), numBlks(numBlocks), alignment(alignment)
{
}

void AllocatorPoolFreeList::init(size_t _blkSize, size_t _numBlocks, size_t _alignment)
{
	blkSize = _blkSize; numBlks = _numBlocks; alignment = _alignment;
	sizeTotal = blkSize * numBlks;

	init();
}

void AllocatorPoolFreeList::init()
{
	if (sizeTotal == 0)
		return;
	
	beginPtr = alignedChunkAlloc(alignment, sizeTotal);
	reset();
}

void* AllocatorPoolFreeList::allocMem(const size_t numAllocatingBlocks /* = 1 */, const size_t alignment /* = 0 */)
{
	if (!rootFreePtr)
	{
		return nullptr;
	}

	if (numUsedBlocks + numAllocatingBlocks > numBlks)
	{
		resize(numBlks * 2);
	}

	if (numAllocatingBlocks == 1)
	{
		++numUsedBlocks;

		void* allocatedPtr = rootFreePtr;
		rootFreePtr = (void**)(*rootFreePtr);
		return allocatedPtr;
	}
	else
	{
		assert("false" && "Allocation multiple blocks is still not implemented");
	}
}

void AllocatorPoolFreeList::freeMem(void* ptr)
{
#if ALLOCATING_DEBUG
	assert(ptr != nullptr);
	assert(!hasOnlyDebugValue(ptr, blkSize), "Attempt to free deleted block yet");
	setDebugValue(ptr, blkSize);

#endif

	*(void**)ptr = rootFreePtr;
	rootFreePtr = (void**)ptr;
	--numUsedBlocks;
}

// NOT SAFE
void AllocatorPoolFreeList::resize(size_t newNumBlocks)
{
	size_t tmpSizeTotal = newNumBlocks * blkSize;
	void* tmpBeginPtr = alignedChunkAlloc(alignment, tmpSizeTotal);
	std::memcpy(tmpBeginPtr, beginPtr, sizeTotal);
	alignedChunkFree(beginPtr);
	
	beginPtr = tmpBeginPtr;
	numBlks = newNumBlocks;
	sizeTotal = tmpSizeTotal;
}

void AllocatorPoolFreeList::reset()
{
	void* curPtr = beginPtr;
	// initialize linked list
	for (size_t i = 0; i < numBlks - 1; ++i)
	{
		void* nextPtr = (char*)curPtr + blkSize;
		*((void**)curPtr) = nextPtr;
		curPtr = nextPtr;
	}

	*((void**)curPtr) = nullptr; // fill the last block

	rootFreePtr = (void**)curPtr;

#if ALLOCATING_DEBUG
	setDebugValue(beginPtr, numBlks*blkSize);
	numUsedBlocks = 0;
#endif
}

AllocatorPoolFreeList::~AllocatorPoolFreeList()
{
#if ALLOCATING_DEBUG
	assert(numUsedBlocks == 0 && "Memory leakage!");
#endif

	alignedChunkFree(beginPtr);
}

}