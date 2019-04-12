#pragma once
#include "pch.h"

#define ALLOCATING_DEBUG true
#if ALLOCATING_DEBUG
#define ALLOC_DEBUG_VALUE 0b10101010
#endif

// Not generate virtual table for Allocator class, because it's a interface class
__declspec(novtable) class Allocator
{
protected:
	size_t sizeTotal;
	size_t sizeUsed = 0;

public:
	Allocator(const size_t totalSize);
	
	virtual void init() = 0;

	virtual void* allocate(const size_t size, const size_t alignment = 0) = 0;
	virtual void free(void* ptr) = 0;

	virtual void reset() = 0;
protected:

#if ALLOCATING_DEBUG
	static void setDebugValue(void* ptr, size_t numBytes);
	static bool hasOnlyDebugValue(void* ptr, size_t numBytes);
#endif
};

