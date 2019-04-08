#include "AllocatorLinear.h"
#include <cstdlib>
#include <assert.h>

AllocatorLinear::AllocatorLinear(const std::size_t totalSize):
	Allocator(totalSize)
{
}

void AllocatorLinear::init()
{
	beginPtr = malloc(sizeTotal);
	reset();
}

void* AllocatorLinear::allocate(const std::size_t size, const std::size_t alignment /* = 0*/)
{
	std::size_t padding = (alignment > 0 ) ? alignment - (uintptr_t)(curPtr) & alignment : 0; // use bitwise and operator for fast mode, because alignment should be power of 2 
	
	if ((uintptr_t)(curPtr) + padding + size > (uintptr_t)(beginPtr) + sizeUsed)
	{
		return nullptr; 
	}

	uintptr_t nextPtr = (uintptr_t)(curPtr) + padding;
	curPtr = (void*)(nextPtr + size);
	sizeUsed += padding + size;

	return (void*)nextPtr;
}

void AllocatorLinear::free(void* ptr)
{
	assert(false && "Can't free an block by ptr. Use reset method");
}

void AllocatorLinear::reset()
{
	curPtr = beginPtr;
	sizeUsed = 0;
}