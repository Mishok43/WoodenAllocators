#include "pch.h"
#include "AllocatorLinear.h"

namespace wal{

AllocatorLinear::AllocatorLinear(const size_t totalSize):
	Allocator(totalSize)
{
}

void AllocatorLinear::init()
{
	beginPtr = malloc(sizeTotal);
	reset();
}

void* AllocatorLinear::allocMem(const size_t size, const size_t alignment /* = 0*/)
{
	size_t padding = (alignment > 0 ) ? computePadding((uintptr_t)curPtr, alignment): 0; // use bitwise and operator for fast mode, because alignment should be power of 2 
	
	if ((uintptr_t)(curPtr) + padding + size > (uintptr_t)(beginPtr) +sizeTotal)
	{
		return nullptr; 
	}

	uintptr_t nextPtr = (uintptr_t)(curPtr) + padding;
	curPtr = (void*)(nextPtr + size);
	sizeUsed += padding + size;

	return (void*)nextPtr;
}

void AllocatorLinear::freeMem(void* ptr)
{
	assert(false && "Can't free an block by ptr. Use reset method");
}

void AllocatorLinear::reset()
{
	curPtr = beginPtr;
	sizeUsed = 0;
}

AllocatorLinear::~AllocatorLinear()
{
	freeMem(beginPtr);
}

}