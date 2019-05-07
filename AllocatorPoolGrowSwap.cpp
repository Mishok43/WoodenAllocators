#include "pch.h"
#include "AllocatorPoolGrowSwap.h"

namespace wal{

AllocatorPoolGrowSwap::AllocatorPoolGrowSwap()
{}

AllocatorPoolGrowSwap::AllocatorPoolGrowSwap(size_t blkSize, size_t numBlocks, size_t alignment) :
	Allocator(blkSize*numBlocks), blkSize(blkSize), numBlks(numBlocks), alignment(alignment)
{
}

void AllocatorPoolGrowSwap::init(size_t _blkSize, size_t _numBlocks, size_t _alignment)
{
	blkSize = _blkSize; numBlks = _numBlocks; alignment = _alignment;
	sizeTotal = blkSize * numBlks;

	init();
}

void AllocatorPoolGrowSwap::init()
{
	if (sizeTotal == 0)
		return;

	beginPtr = alignedChunkAlloc(alignment, sizeTotal);
	reset();
}

void* AllocatorPoolGrowSwap::allocMem(const size_t numAllocatingBlocks /* = 1 */, const size_t alignment /* = 0 */)
{
	if (numUsedBlocks + numAllocatingBlocks > numBlks)
	{
		resize(numBlks * 2);
	}

	if (numAllocatingBlocks == 1)
	{
		return (char*)beginPtr + blkSize*numUsedBlocks++;
	}
	else
	{
		assert("false" && "Allocation multiple blocks is still not implemented");
	}
}

size_t AllocatorPoolGrowSwap::freeMem(void* ptr)
{
#if ALLOCATING_DEBUG
	assert(ptr != nullptr);
	assert(!hasOnlyDebugValue(ptr, blkSize), "Attempt to free deleted block yet");
#endif

	size_t iSwap = numUsedBlocks - 1;
	void* swapPtr = get(numUsedBlocks - 1);

	std::memcpy(ptr, swapPtr, blkSize);

#if ALLOCATING_DEBUG
	setDebugValue(swapPtr, blkSize);
#endif
	--numUsedBlocks;

	return iSwap;
}

// NOT SAFE
void AllocatorPoolGrowSwap::resize(size_t newNumBlocks)
{
	size_t tmpSizeTotal = newNumBlocks * blkSize;
	void* tmpBeginPtr = alignedChunkAlloc(alignment, tmpSizeTotal);
	std::memcpy(tmpBeginPtr, beginPtr, sizeTotal);
	alignedChunkFree(beginPtr);
	
	beginPtr = tmpBeginPtr;
	numBlks = newNumBlocks;
	sizeTotal = tmpSizeTotal;
}

void AllocatorPoolGrowSwap::reset()
{
#if ALLOCATING_DEBUG
	setDebugValue(beginPtr, numBlks*blkSize);
#endif

	numUsedBlocks = 0;
}


AllocatorPoolGrowSwap::~AllocatorPoolGrowSwap()
{
#if ALLOCATING_DEBUG
	assert(numUsedBlocks == 0 && "Memory leakage!");
#endif

	alignedChunkFree(beginPtr);
}
}