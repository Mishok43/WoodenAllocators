#include "pch.h"
#include "AllocatorStack.h"

namespace wal{

AllocatorStack::AllocatorStack(const size_t totalSize, const size_t numChunksMax) :
	Allocator(totalSize), numChunksMax(numChunksMax)
{}


void AllocatorStack::init()
{
	beginPtr = malloc(sizeTotal);
	sizeData = 0;

	reset();
}


void* AllocatorStack::allocMem(const size_t size, const size_t alignment /* = 0*/)
{
	size_t padding = (alignment > 0) ? computePadding((uintptr_t)curPtr, alignment): 0; // use bitwise and operator for fast mode, because alignment should be power of 2 

	if ((uintptr_t)(curPtr)+padding + size > (uintptr_t)(beginPtr)+sizeUsed)
	{
		return nullptr;
	}

	uintptr_t nextPtr = (uintptr_t)(curPtr)+padding;
	curPtr = (void*)(nextPtr + size);
	sizeUsed += padding + size;

	sizeData[numChunks++] = padding + size;

	return (void*)nextPtr;
}


void AllocatorStack::freeMem(void* ptr)
{
#if ALLOCATING_DEBUG
	assert(numChunks > 0 && "stack must be not empty!");

	--numChunks;
	uint32_t totalSize = sizeData[numChunks];

	assert(totalSize > sizeUsed);
	assert(totalSize >= (uintptr_t)curPtr - (uintptr_t)ptr);
		
	uint32_t padding = totalSize - (uintptr_t)curPtr - (uintptr_t)ptr;

	curPtr = (void*)((uintptr_t)ptr - padding);
	assert(!hasOnlyDebugValue(curPtr, totalSize), "Attempt to free deleted block yet");

	sizeUsed -= totalSize;

	setDebugValue(curPtr, totalSize);
	setDebugValue(sizeData+numChunks, sizeof(uint32_t));
#else
	--numChunks;
	uint32_t totalSize = sizeData[numChunks];
	uint32_t padding =  totalSize - (uintptr_t)curPtr - (uintptr_t)ptr;
	curPtr = (void*)((uintptr_t)ptr-padding);
	sizeUsed -= totalSize;
#endif
}


void AllocatorStack::reset()
{
	curPtr = beginPtr;
	sizeUsed = 0;
	numChunks = 0;

#if ALLOCATING_DEBUG
	setDebugValue(beginPtr, sizeTotal);
	setDebugValue(sizeData, numChunksMax * sizeof(uint32_t));
#endif 
}


AllocatorStack::~AllocatorStack()
{
#if ALLOCATING_DEBUG
	assert(curPtr == beginPtr);
#endif

	freeMem(beginPtr);
	freeMem(sizeData);
}

}