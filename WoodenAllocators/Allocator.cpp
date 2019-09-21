#include "pch.h"
#include "Allocator.h"

namespace wal{

Allocator::Allocator()
{ }

Allocator::Allocator(const size_t totalSize) :
	sizeTotal(totalSize)
{}

size_t Allocator::getSize() const
{
	return sizeTotal;
}

void* Allocator::alignedChunkAlloc(uint32_t alignment, uint32_t size)
{
	void* ptr = malloc(size + alignment);
	uint8_t padding = computePadding((uintptr_t)ptr, alignment);
	if (padding == 0)
	{
		padding = alignment;
	}

	ptr = (void*)((uintptr_t)(ptr)+padding);

	uint8_t* headerDataPtr = (uint8_t*)((uintptr_t)ptr - 1);
	*headerDataPtr = padding;
	return ptr;
}

void Allocator::alignedChunkFree(void* ptr)
{
	uint8_t * headerDataPtr = (uint8_t*)((uintptr_t)ptr - 1);
	uint8_t padding = *headerDataPtr;
	ptr = (void*)((uintptr_t)ptr - padding);
	free(ptr);
}

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

}