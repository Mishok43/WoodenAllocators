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
	assert(alignment == 0, "Alignment not supported!");
#endif
	neededSize = (blkSizeType)blkSize + ALLOCATED_HEADER_DATA_SIZE;

	FreeHeaderData* blkPtr = beginPtr;
	FreeHeaderData* prevBlkPtr;
	while (blkPtr->next!= 0)
	{
		if (blkPtr->size >= neededSize)
		{
			break;
		}
		prevBlkPtr = blkPtr;
		blkPtr = (FreeHeaderData*)((char*)beginPtr + blkPtr->next);
	}

	if (blkPtr->size < neededSize)
	{
		return nullptr;
	}


	char* endBlkPtr = (char*)blkPtr + blkPtr->size;
	char* curPtr;


	// if it's not possible to divide block to 2 blocks, because of unsuffiecent size, just use it fully. 
	if (blkPtr->size - neededSize >= FREE_HEADER_DATA_SIZE)
	{
		blkPtr->size -= neededSize;
		curPtr = endBlkPtr - neededSize;
		((AllocatedHeaderData*)curPtr)->size = neededSize;
	}
	else
	{
		curPtr = (char*)blkPtr;
		prevBlkPtr->next= blkPtr->next;
		((AllocatedHeaderData*)curPtr)->size = blkPtr->size;
	}
	return curPtr + ALLOCATED_HEADER_DATA_SIZE;
}

template<typename blkSizeType>
void AllocatorFreeList<blkSizeType>::free(void* ptr)
{
	AllocatedHeaderData* curPtr = (AllocatedHeaderData*)((char*)ptr - ALLOCATED_HEADER_DATA_SIZE);
#if ALLOCATING_DEBUG
	assert(!hasOnlyDebugValue(curPtr, ALLOCATED_HEADER_DATA_SIZE));
#endif

	FreeHeaderData* prevFreePtr = beginPtr;
	while (prevFreePtr->next!= 0)
	{
		char* nextFreePtr = (char*)beginPtr + prevFreePtr->next;
		if ((uintptr_t)nextFreePtr < (uintptr_t)curPtr)
		{
			prevFreePtr = (FreeHeaderData*)nextFreePtr;
		}
		else
		{
			break;
		}
	}

#if ALLOCATING_DEBUG
	assert(!hasOnlyDebugValue((char*)curPtr + ALLOCATED_HEADER_DATA_SIZE, curPtr->size));
#endif

	// Merge prev and cur free blocks if it's possible
	if ((char*)prevFreePtr + prevFreePtr->size == curPtr)
	{
		prevFreePtr->size += curPtr->size;
		prevFreePtr->next= curPtr->next;
#if ALLOCATING_DEBUG
		setDebugValue((char*)curPtr, curPtr->size+ALLOCATED_HEADER_DATA_SIZE);
#endif
		curPtr = prevFreePtr;
	}
	else
	{
		FreeHeaderData* curFreePtr = (FreeHeaderData*)curPtr;
		curFreePtr->next= prevFreePtr->next;
		prevFreePtr->next= (uintptr_t)curFreePtr - (uintptr_t)beginPtr;
#if ALLOCATING_DEBUG
		setDebugValue((char*)curPtr + ALLOCATED_HEADER_DATA_SIZE, curPtr->size);
#endif
	}

	// Merge cur and next free blocks if it's possible
	if (curPtr->nextOffset != 0)
	{
		FreeHeaderData* nextFreePtr = (char*)beginPtr + curPtr->nextOffset;
		if ((char*)curPtr + curPtr->size == nextFreePtr)
		{
			curPtr->size += nextFreePtr->size;
			curPtr->nextOffset = nextFreePtr->nextOffset;
		}
	}
}


template<typename blkSizeType>
void AllocatorFreeList<blkSizeType>::reset()
{
#if ALLOCATING_DEBUG
	setDebugValue(beginPtr, sizeTotal);
#endif

	beginPtr->next = 0;
	beginPtr->size = sizeTotal;
}

template<typename blkSizeType>
AllocatorFreeList<blkSizeType>::~AllocatorFreeList()
{
#if ALLOCATING_DEBUG
	assert(beginPtr->size == sizeTotal && "Memory leak");
	assert(hasOnlyDebugValue((char*)beginPtr + FREE_HEADER_DATA_SIZE, beginPtr->size - FREE_HEADER_DATA_SIZE) && "Memory leak or ub");
#endif

	free(beginPtr);
}
