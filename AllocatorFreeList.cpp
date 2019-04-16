#include "pch.h"
#include "AllocatorFreeList.h"

template<typename blkSizeType>
AllocatorFreeList<blkSizeType>::AllocatorFreeList(const blkSizeType totalSize) :
	Allocator(totalSize)
{}

template<typename blkSizeType>
void AllocatorFreeList<blkSizeType>::init()
{
	beginPtr = (HeaderData*)malloc(sizeTotal);
	reset();
}

template<typename blkSizeType>
void* AllocatorFreeList<blkSizeType>::allocate(const size_t blkSize/* =1 */, const size_t alignment /* = 0 */)
{
	blkSizeType neededSize;
#if ALLOCATING_DEBUG
	assert(blkSize <= sizeTotal);
#endif
	neededSize = (blkSizeType)blkSize;

	HeaderData* curPtr = beginPtr;
	while(curPtr->offsetNext != 0)
	{
		if (curPtr->size > neededSize)
		{
			break;
		}
		curPtr = (HeaderData*)((char*)curPtr + curPtr->offsetNext);
	}

	if (curPtr->size < neededSize)
	{
		return nullptr;
	}

	uint8_t headerSize = sizeof(HeaderData); 

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
