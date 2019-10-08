#pragma once

#include "Allocator.h"

namespace wal{

class AllocatorPoolFreeList: public Allocator
{
public:
	AllocatorPoolFreeList();

	AllocatorPoolFreeList(size_t blkSize, size_t numBlocks, size_t alignment);
	~AllocatorPoolFreeList();

	void init(size_t blkSize, size_t numBlocks, size_t alignment);
	void init();

	void resize(size_t numBlks);
	void* allocMem(const size_t numBlocks=1, const size_t alignment = 0);
	void freeMem(void* ptr);

	void reset();

	inline size_t getBlkSize() const noexcept { return blkSize; }

	inline size_t getNumUsedBlks() const noexcept {return numUsedBlocks; }
	inline size_t getNumBlks() const noexcept {return numBlks; }

	template<typename T=char>
	inline T* get(size_t pos) { return &((T*)beginPtr)[pos]; }

	template<typename T =char>
	inline const T* get(size_t pos) const { return *((T*)beginPtr)[pos];}

	inline size_t getPos(void* blk)
	{
		assert((uintptr_t)blk >= (uintptr_t)beginPtr && (uintptr_t)blk <= (uintptr_t)beginPtr + numBlks * blkSize);
		return ((uintptr_t)blk - (uintptr_t)beginPtr) / blkSize;
	}

	inline void* data(){	return beginPtr; }

	inline const void* data() const { return beginPtr;}
private:
	void* beginPtr;
	uint32_t* rootFreePtr;

	size_t blkSize;
	size_t numBlks;
	size_t alignment;
	size_t numUsedBlocks;
};


}
