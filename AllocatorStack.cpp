#include "AllocatorStack.h"


AllocatorStack::AllocatorStack(const size_t totalSize, const size_t numChunksMax) :
	Allocator(totalSize), numChunksMax(numChunksMax)
{}


void AllocatorStack::init()
{
	beginPtr = malloc(sizeTotal);
	sizeData = aligned_alloc(sizeof(uint32_t), numChunksMax*sizeof(uint32_t));

#if ALLOCATING_DEBUG
	std::memset(beginPtr, 0, sizeTotal);
	std::memset(sizeData, 0, numChunksMax * sizeof(uint32_t));
#endif 

	reset();
}



void* AllocatorStack::allocate(const size_t size, const size_t alignment /* = 0*/)
{
	size_t padding = (alignment > 0) ? alignment - (uintptr_t)(curPtr)& alignment : 0; // use bitwise and operator for fast mode, because alignment should be power of 2 

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


void AllocatorStack::free(void* ptr)
{
#if ALLOCATING_DEBUG
	assert(numChunks > 0 && "stack must be not empty!");
	--numChunks;
	uint32_t totalSize = sizeData[numChunks];

	assert(totalSize > sizeUsed);
	assert(totalSize >= (uintptr_t)curPtr - (uintptr_t)ptr);
		
	uint32_t padding = totalSize - (uintptr_t)curPtr - (uintptr_t)ptr;

	curPtr = (void*)((uintptr_t)ptr - padding);
	sizeUsed -= totalSize;

	std::memset(curPtr, 0, totalSize);
	sizeData[numChunks] = 0;
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
}


AllocatorStack::~AllocatorStack()
{
#if ALLOCATING_DEBUG
	for (size_t i = 0; i < sizeTotal; ++i)
	{
		assert(((char*)beginPtr)[i] == 0);
	}

	for (size_t i = 0; i < numChunksMax; ++i)
	{
		assert(sizeData[i] == 0);
	}

	assert(curPtr == beginPtr);
#endif

	free(beginPtr);
	free(sizeData);
}