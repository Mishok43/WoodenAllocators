#pragma once

#include "Allocator.h"



class AllocatorPoolFreeList: public Allocator
{
public:
	AllocatorPoolFreeList(const size_t blkSize, const size_t numBlocks, const size_t alignment);
	~AllocatorPoolFreeList();

	void init() override;
	
	void* allocate(const size_t numBlocks=1, const size_t alignment = 0) override;
	void free(void* ptr) override;

	void reset() override;

private:
	void* beginPtr;
	void** rootFreePtr;

	const size_t blkSize;
	const size_t numBlocks;

#if ALLOCATING_DEBUG
	size_t numFreeBlocks;
#endif
};

