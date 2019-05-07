#include "pch.h"
#include "AllocatorPoolGrowFreeList.h"

namespace wal{

AllocatorPoolGrowFreeList::AllocatorPoolGrowFreeList()
{}

AllocatorPoolGrowFreeList::AllocatorPoolGrowFreeList(size_t blkSize, size_t numBlocks, size_t alignment) :
	Allocator(blkSize*numBlocks), blkSize(blkSize), numBlks(numBlocks), alignment(alignment)
{
}

void AllocatorPoolGrowFreeList::init(size_t _blkSize, size_t _numBlocks, size_t _alignment)
{
	blkSize = _blkSize; numBlks = _numBlocks; alignment = _alignment;
	sizeTotal = blkSize * numBlks;

	init();
}

void AllocatorPoolGrowFreeList::init()
{
	if (sizeTotal == 0)
		return;
	
	beginPtr = alignedChunkAlloc(alignment, sizeTotal);
	reset();
}

void* AllocatorPoolGrowFreeList::allocMem(const size_t numAllocatingBlocks /* = 1 */, const size_t alignment /* = 0 */)
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

void AllocatorPoolGrowFreeList::freeMem(void* ptr)
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
void AllocatorPoolGrowFreeList::resize(size_t newNumBlocks)
{
	size_t tmpSizeTotal = newNumBlocks * blkSize;
	void* tmpBeginPtr = alignedChunkAlloc(alignment, tmpSizeTotal);
	std::memcpy(tmpBeginPtr, beginPtr, sizeTotal);
	alignedChunkFree(beginPtr);
	
	beginPtr = tmpBeginPtr;
	numBlks = newNumBlocks;
	sizeTotal = tmpSizeTotal;
}

void AllocatorPoolGrowFreeList::reset()
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

AllocatorPoolGrowFreeList::~AllocatorPoolGrowFreeList()
{
#if ALLOCATING_DEBUG
	assert(numUsedBlocks == 0 && "Memory leakage!");
#endif

	alignedChunkFree(beginPtr);
}

}