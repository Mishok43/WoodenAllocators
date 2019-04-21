#pragma once
#include "Allocator.h"

class AllocatorFreeListTree: public Allocator
{
public:
	AllocatorFreeListTree(const uint32_t totalSize);
	~AllocatorFreeListTree();

	void init() override;

	void* allocate(const size_t blkSize = 1, const size_t alignment = 0) override;
	void free(void* ptr) override;

	void reset() override;
private:
	struct FreeHeaderData;

// disable padding in structures because they won't be aligned anyway
#pragma pack(push, 1)
	struct AllocatedHeaderData
	{
		FreeHeaderData* next;
		FreeHeaderData* prev;
		bool bAllocatd;
	};
#pragma pack(pop)

#pragma pack(push, 1)
	struct FreeHeaderData: public AllocatedHeaderData
	{
		bool bRed;
		FreeHeaderData* parent;
		FreeHeaderData* right;
		FreeHeaderData* left;
	};
#pragma pack(pop)



	FreeHeaderData* beginPtr;
	FreeHeaderData* root;

	FreeHeaderData* findBestFitFreeBlk(size_t blkSize) const;
	FreeHeaderData* minFreeBlk(FreeHeaderData* node) const;
	FreeHeaderData* maxFreeBlk(FreeHeaderData* node) const;
	FreeHeaderData* successorFreeBlk(FreeHeaderData* node) const;
	FreeHeaderData* predecessorFreeBlk(FreeHeaderData* node) const;
	void insertFreeBlk(FreeHeaderData* data);
	void deleteFreeBlk(FreeHeaderData* node);

	inline uint32_t getBlkSize(void* blk) const;
	
	const static uint8_t FREE_HEADER_DATA_SIZE = sizeof(FreeHeaderData);
	const static uint8_t ALLOCATED_HEADER_DATA_SIZE = sizeof(AllocatedHeaderData);
};


