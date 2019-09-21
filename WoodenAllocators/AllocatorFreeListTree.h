#pragma once
#include "Allocator.h"

namespace wal{

class AllocatorFreeListTree: public Allocator
{
public:
	AllocatorFreeListTree(const uint32_t totalSize);
	~AllocatorFreeListTree();

	void init();

	void* allocMem(const size_t blkSize = 1);
	void freeMem(void* ptr);

	void reset();
private:
	struct FreeHeaderData;

// disable padding in structures because they won't be aligned anyway
#pragma pack(push, 1)
	struct AllocatedHeaderData
	{
		FreeHeaderData* next;
		FreeHeaderData* prev;
		bool bAllocated;
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

	static FreeHeaderData NIL;

	FreeHeaderData* beginPtr;
	FreeHeaderData* root;

	inline bool			isNIL(FreeHeaderData*  blk) const noexcept;
	FreeHeaderData* findBestFitFreeBlk(size_t blkSize) const;
	FreeHeaderData* minFreeBlk(FreeHeaderData* node) const;
	FreeHeaderData* maxFreeBlk(FreeHeaderData* node) const;
	FreeHeaderData* successorFreeBlk(FreeHeaderData* node) const;
	FreeHeaderData* predecessorFreeBlk(FreeHeaderData* node) const;
	void insertFixUpTree(FreeHeaderData* node);
	void deleteFixUpTree(FreeHeaderData* node);
	void rotateLeftFreeBlk(FreeHeaderData* node);
	void rotateRightFreeBlk(FreeHeaderData* node);
	void insertFreeBlk(FreeHeaderData* data);
	void deleteFreeBlk(FreeHeaderData* node);
	void transitFreeBlk(FreeHeaderData* node, FreeHeaderData* child);

	inline uint32_t getBlkSize(void* blk) const;
	inline void* getAfterFreeHeaderPtr(void* ptr) const;
	inline void* getAfterAllocatedHeaderPtr(void* ptr) const;
	
	const static uint8_t FREE_HEADER_DATA_SIZE = sizeof(FreeHeaderData);
	const static uint8_t ALLOCATED_HEADER_DATA_SIZE = sizeof(AllocatedHeaderData);
};


}