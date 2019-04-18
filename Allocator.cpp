#include "pch.h"
#include "Allocator.h"

Allocator::Allocator(const size_t totalSize) :
	sizeTotal(totalSize)
{}


#if ALLOCATING_DEBUG
void Allocator::setDebugValue(void* ptr, size_t numBytes)
{
	std::memset(ptr, ALLOC_DEBUG_VALUE, numBytes);
}

bool Allocator::hasOnlyDebugValue(void* ptr, size_t numBytes)
{
	bool onlyDebugValue = false;
	for (size_t i = 0; i < numBytes; ++i)
	{
		onlyDebugValue = ((char*)ptr)[i] == ALLOC_DEBUG_VALUE;
	}

	return onlyDebugValue;
}

inline uint32_t Allocator::computePadding(uintptr_t ptr, uint32_t alignment) const
{
	return alignment - ptr& alignment; // use bitwise and operator for fast mode, because alignment should be power of 2 
}

#endif

