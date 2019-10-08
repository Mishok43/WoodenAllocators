#include "pch.h"
#include "AllocatorPoolFreeList.h"

namespace wal{

AllocatorPoolFreeList::AllocatorPoolFreeList():
	rootFreePtr(nullptr)
{}

AllocatorPoolFreeList::AllocatorPoolFreeList(size_t blkSize, size_t numBlocks, size_t alignment) :
	Allocator(blkSize*numBlocks), blkSize(blkSize), numBlks(numBlocks), alignment(alignment), rootFreePtr(nullptr)
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
		rootFreePtr =  (uint32_t*)((char*)beginPtr+(*rootFreePtr));
		return allocatedPtr;
	}
	else
	{
		assert("false" && "Allocation multiple blocks is still not implemented");
	}
}

void AllocatorPoolFreeList::freeMem(void* ptr)
{

	*(uint32_t*)ptr = getPos(rootFreePtr)*blkSize;
	rootFreePtr = (uint32_t*)ptr;
	--numUsedBlocks;
}

// NOT SAFE
void AllocatorPoolFreeList::resize(size_t newNumBlocks)
{
	size_t tmpSizeTotal = newNumBlocks * blkSize;
	void* tmpBeginPtr = alignedChunkAlloc(alignment, tmpSizeTotal);
	std::memcpy(tmpBeginPtr, beginPtr, sizeTotal);
	alignedChunkFree(beginPtr);

	void* splitPtr = (char*)tmpBeginPtr + sizeTotal;
	// initialize linked list
	void* curPtr = splitPtr;

	uint32_t offset = sizeTotal;
	for (size_t i = 0; i < newNumBlocks-numBlks - 1; ++i)
	{
		offset += blkSize;
		void* nextPtr = (char*)curPtr + blkSize;
		*((uint32_t*)curPtr) = offset;
		curPtr = nextPtr;
	}
	*((uint32_t*)curPtr) = (uint32_t)-1; // fill the last block
	rootFreePtr = (uint32_t*)splitPtr;
	
	beginPtr = tmpBeginPtr;
	numBlks = newNumBlocks;
	sizeTotal = tmpSizeTotal;
}

void AllocatorPoolFreeList::reset()
{
	void* curPtr = beginPtr;
	// initialize linked list
	uint32_t offset = 0;
	for (size_t i = 0; i < numBlks - 1; ++i)
	{
		offset += blkSize;
		void* nextPtr = (char*)curPtr + blkSize;
		*((uint32_t*)curPtr) = offset;
		curPtr = nextPtr;
	}

	*((uint32_t*)curPtr) = (uint32_t)-1; // fill the last block

	rootFreePtr = (uint32_t*)beginPtr;
}

AllocatorPoolFreeList::~AllocatorPoolFreeList()
{

	alignedChunkFree(beginPtr);
}

}