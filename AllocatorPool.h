#pragma once

#include "Allocator.h"

class AllocatorPool: public Allocator
{
public:
	AllocatorPool(const size_t blkSize, const size_t numBlocks);
	~AllocatorPool();

	void init() override;
	
	void* allocate(const size_t numBlocks, const std::size_t alignment = 0) override;
	void free(void* ptr) override;

	void reset() override;

private:
	void* beginPtr;
	void* rootFree;

	const size_t blkSize;
	const size_t numBlocks;
};

