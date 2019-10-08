#include "pch.h"
#include "AllocatorPoolSwap.h"

namespace wal{

AllocatorPoolSwap::AllocatorPoolSwap()
{}

AllocatorPoolSwap::AllocatorPoolSwap(size_t blkSize, size_t numBlocks, size_t alignment) :
	Allocator(blkSize*numBlocks), blkSize(blkSize), numBlks(numBlocks), alignment(alignment)
{
}

void AllocatorPoolSwap::init(size_t _blkSize, size_t _numBlocks, size_t _alignment)
{
	blkSize = _blkSize; numBlks = _numBlocks; alignment = _alignment;
	sizeTotal = blkSize * numBlks;

	init();
}

void AllocatorPoolSwap::init()
{
	if (sizeTotal == 0)
		return;

	beginPtr = alignedChunkAlloc(alignment, sizeTotal);
	reset();
}



size_t AllocatorPoolSwap::freeMem(void* ptr)
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



void AllocatorPoolSwap::reset()
{
#if ALLOCATING_DEBUG
	setDebugValue(beginPtr, numBlks*blkSize);
#endif

	numUsedBlocks = 0;
}


AllocatorPoolSwap::~AllocatorPoolSwap()
{
#if ALLOCATING_DEBUG
	//	assert(numUsedBlocks == 0 && "Memory leakage!");
#endif

	alignedChunkFree(beginPtr);
}
}