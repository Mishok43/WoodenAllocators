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


#endif

