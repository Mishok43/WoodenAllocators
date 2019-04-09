#pragma once
#include "pch.h"

#define ALLOCATING_DEBUG true

__declspec(novtable) class Allocator
{
protected:
	std::size_t sizeTotal;
	std::size_t sizeUsed = 0;

public:
	Allocator(const std::size_t totalSize);
	
	virtual void init() = 0;

	virtual void* allocate(const std::size_t size, const std::size_t alignment = 0) = 0;
	virtual void free(void* ptr) = 0;

	virtual void reset() = 0;
};

