#pragma once
#include "Allocator.h"

namespace wal{

template<typename blkSizeType=uint16_t>
class AllocatorFreeList: public Allocator
{
public:
	AllocatorFreeList(const blkSizeType totalSize);
	~AllocatorFreeList();

	void init();
	
	void* allocMem(const size_t blkSize=1);
	void freeMem(void* ptr);

	void reset();
private:
	struct FreeHeaderData
	{
		blkSizeType size;
		blkSizeType next;
	};

	struct AllocatedHeaderData
	{
		blkSizeType size;
	};


	FreeHeaderData* beginPtr;

	const static uint8_t FREE_HEADER_DATA_SIZE = sizeof(FreeHeaderData);
	const static uint8_t ALLOCATED_HEADER_DATA_SIZE = sizeof(AllocatedHeaderData);
};


}