#pragma once

#include "Allocator.h"

namespace wal{

class AllocatorStack : public Allocator
{
public:
	AllocatorStack(const size_t totalSize, const size_t numChunksMax);

	~AllocatorStack();

	void init();

	void* allocMem(const size_t size, const size_t alignment = 0);
	void freeMem(void* ptr);

	void reset();

private:
	void* beginPtr = nullptr;
	void* curPtr = nullptr;
	
	const uint32_t numChunksMax;

	uint32_t numChunks = 0;
	uint32_t* sizeData = nullptr; // store the headers data separately for increasing # cache hits
};

}

