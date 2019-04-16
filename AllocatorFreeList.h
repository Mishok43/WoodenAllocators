#pragma once
#include "Allocator.h"

template<typename blkSizeType=uint16_t>
class AllocatorFreeList: public Allocator
{
public:
	AllocatorFreeList(const blkSizeType totalSize);
	~AllocatorFreeList();

	void init() override;
	
	void* allocate(const size_t blkSize=1, const size_t alignment = 0) override;
	void free(void* ptr) override;

	void reset() override;
private:
	struct HeaderData
	{
		blkSizeType offsetNext;
		blkSizeType size;
	};

	HeaderData* beginPtr;


#if ALLOCATING_DEBUG
	size_t numFreeBlocks;
#endif
};


