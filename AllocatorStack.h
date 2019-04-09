#pragma once

#include "Allocator.h"

class AllocatorStack : public Allocator
{
public:
	AllocatorStack(const size_t totalSize, const size_t numChunksMax);

	~AllocatorStack();

	void init() override;

	void* allocate(const size_t size, const size_t alignment = 0) override;
	void free(void* ptr) override;

	void reset() override;

private:
	void* beginPtr = nullptr;
	void* curPtr = nullptr;
	
	const uint32_t numChunksMax;

	uint32_t numChunks = 0;
	uint32_t* sizeData = nullptr; // store the headers data separately for increasing # cache hits
};

