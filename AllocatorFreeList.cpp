#include "pch.h"
#include "AllocatorFreeList.h"

template<typename blkSizeType>
AllocatorFreeList<blkSizeType>::AllocatorFreeList(const blkSizeType totalSize) :
	Allocator(totalSize)
{}

template<typename blkSizeType>
void AllocatorFreeList<blkSizeType>::init()
{
	beginPtr = (FreeHeaderData*)malloc(sizeTotal);
	reset();
}

template<typename blkSizeType>
void* AllocatorFreeList<blkSizeType>::allocate(const size_t blkSize/* =1 */, const size_t alignment /* = 0 */)
{
	blkSizeType neededSize;
#if ALLOCATING_DEBUG
	assert(blkSize <= sizeTotal);
#endif
	neededSize = (blkSizeType)blkSize+ALLOCATED_HEADER_DATA_SIZE+alignment;

	FreeHeaderData* blkPtr = beginPtr;
	while(blkPtr->offsetNext != 0)
	{
		if (blkPtr->size >= neededSize)
		{
			break;
		}
		blkPtr = (FreeHeaderData*)((char*)blkPtr + blkPtr->offsetNext);
	}

	if (blkPtr->size < neededSize)
	{
		return nullptr;
	}

	
	neededSize -= alignment;

	char* endPtr = (char*)blkPtr + blkPtr->size;
	char* curPtr = endPtr - neededSize;

	blkSizeType padding = computePadding((uintptr_t)curPtr, alignment);

	// if it's not possible to divide block to 2 blocks, because of unsuffiecent size, just use it fully. TODO: merge the free blocks to the next block if it's possible
	if ((uintptr_t)curPtr - padding - (uintptr_t)blkPtr < FREE_HEADER_DATA_SIZE)
	{
		curPtr = (char*)blkPtr;
		padding = computePadding((uintptr_t)curPtr, alignment);
		*(AllocatedHeaderData*)curPtr = blkPtr->size;
	}
	else
	{

	}



	// try allocate memory in the end of the block

}

template<typename blkSizeType>
void AllocatorFreeList<blkSizeType>::reset()
{
#if ALLOCATING_DEBUG
	setDebugValue(beginPtr, sizeTotal);
#endif

	beginPtr->offsetNext = 0;
	beginPtr->size = sizeTotal;
}
