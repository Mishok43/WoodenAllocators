#pragma once
#include "pch.h"
	

namespace wal{

// Not generate virtual table for Allocator class, because it's a interface class
class Allocator
{
protected:
	size_t sizeTotal;
	size_t sizeUsed;

public:
	Allocator();
	Allocator(const size_t totalSize);

	size_t getSize() const;

	inline static uint32_t computePadding(uintptr_t ptr, uint32_t alignment)
	{
		uint32_t padding = ptr & alignment;
		return (padding == 0) ? 0 : alignment - padding;
	}


	static void* alignedChunkAlloc(uint32_t alignment, uint32_t size);

	static void alignedChunkFree(void* ptr);
protected:

#if ALLOCATING_DEBUG
	static void setDebugValue(void* ptr, size_t numBytes);
	static bool hasOnlyDebugValue(void* ptr, size_t numBytes);
#endif
};

}