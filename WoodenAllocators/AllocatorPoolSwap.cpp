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

void* AllocatorPoolSwap::allocMem(const size_t numAllocatingBlocks /* = 1 */, const size_t alignment /* = 0 */)
{
	if (numUsedBlocks + numAllocatingBlocks > numBlks)
	{
		resize(numBlks * 2);
	}

	if (numAllocatingBlocks == 1)
	{
		return (char*)beginPtr + blkSize*numUsedBlocks++;
	}
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

// NOT SAFE
void AllocatorPoolSwap::resize(size_t newNumBlocks)
{
	size_t tmpSizeTotal = newNumBlocks * blkSize;
	void* tmpBeginPtr = alignedChunkAlloc(alignment, tmpSizeTotal);
	std::memcpy(tmpBeginPtr, beginPtr, sizeTotal);
	alignedChunkFree(beginPtr);
	
	beginPtr = tmpBeginPtr;
	numBlks = newNumBlocks;
	sizeTotal = tmpSizeTotal;
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
	assert(numUsedBlocks == 0 && "Memory leakage!");
#endif

	alignedChunkFree(beginPtr);
}
}