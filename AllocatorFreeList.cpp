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
	assert(alignment == 0 || ALLOCATED_HEADER_DATA_SIZE <= alignment);
#endif
	neededSize = (alignment > 0) ? (blkSizeType)blkSize + alignment: (blkSizeType)blkSize + ALLOCATED_HEADER_DATA_SIZE;

	FreeHeaderData* blkPtr = beginPtr;
	FreeHeaderData* prevBlkPtr;
	while (blkPtr->offsetNext != 0)
	{
		if (blkPtr->size >= neededSize)
		{
			break;
		}
		prevBlkPtr = blkPtr;
		blkPtr = (FreeHeaderData*)((char*)blkPtr + blkPtr->offsetNext);
	}

	if (blkPtr->size < neededSize)
	{
		return nullptr;
	}


	char* endBlkPtr = (char*)blkPtr + blkPtr->size;
	char* curPtr;

	if (alignment == 0)
	{
		// if it's not possible to divide block to 2 blocks, because of unsuffiecent size, just use it fully. 
		if (blkPtr->size - neededSize >= FREE_HEADER_DATA_SIZE)
		{
			blkPtr->size -= neededSize;
			curPtr = endBlkPtr - neededSize;
			((AllocatedHeaderData*)curPtr)->size = blkSize;
		}
		else
		{
			curPtr = (char*)blkPtr;
			prevBlkPtr->offsetNext = blkPtr->offsetNext;
			((AllocatedHeaderData*)curPtr)->size = blkPtr->size - ALLOCATED_HEADER_DATA_SIZE;
		}
		return curPtr + ALLOCATED_HEADER_DATA_SIZE;
	}
	else
	{
		neededSize -= alignment;
		curPtr = endBlkPtr - neededSize;
		blkSizeType padding = alignment - computePadding((uintptr_t)curPtr, alignment);
		if (blkPtr->size - padding - neededSize >= FREE_HEADER_DATA_SIZE)
		{
			curPtr -= padding;
			blkPtr->size -= padding - neededSize;
			((AllocatedHeaderData*)curPtr)->size = blkSize;
		}
		else
		{
			curPtr = (char*)blkPtr;
			padding = computePadding((uintptr_t)curPtr, alignment);
			
		}





		neededSize += ALLOCATED_HEADER_DATA_SIZE;
		neededSize -= alignment - ALLOCATED_HEADER_DATA_SIZE;

		blkSizeType padding = alignment - computePadding((uintptr_t)curPtr, alignment);
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
