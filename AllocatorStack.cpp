#include "AllocatorStack.h"
#include <cstdlib>
#include <assert.h>


template<bool safe>
AllocatorStack<safe>::AllocatorStack(const std::size_t totalSize) :
	Allocator(totalSize)
{
}

template<bool safe>
void AllocatorStack<safe>::init()
{
	beginPtr = malloc(sizeTotal);
	reset();
}


template<bool safe>
void* AllocatorStack<safe>::allocate(const std::size_t size, const std::size_t alignment /* = 0*/)
{
	std::size_t padding = (alignment > 0) ? alignment - (uintptr_t)(curPtr)& alignment : 0; // use bitwise and operator for fast mode, because alignment should be power of 2 
	if (padding < HEADER_SIZE)
	{
		padding += HEADER_SIZE - padding;
	}

	if ((uintptr_t)(curPtr)+padding + size > (uintptr_t)(beginPtr)+sizeUsed)
	{
		return nullptr;
	}

	uintptr_t nextPtr = (uintptr_t)(curPtr)+padding;
	curPtr = (void*)(nextPtr + size);
	sizeUsed += padding + size;

	return (void*)nextPtr;
}

template<bool safe>
void AllocatorStack<safe>::free(void* ptr)
{
	if (safe)
	{

	}
	else
	{
		curPtr -= 
	}
}

template<bool safe>
void AllocatorStack<safe>::reset()
{
	curPtr = beginPtr;
	sizeUsed = 0;
}