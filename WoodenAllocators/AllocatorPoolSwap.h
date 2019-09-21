#pragma once
#include "Allocator.h"

namespace wal{

class AllocatorPoolSwap: public Allocator
{
public:
	AllocatorPoolSwap();

	AllocatorPoolSwap(size_t blkSize, size_t numBlocks, size_t alignment=0);
	~AllocatorPoolSwap();

	void init(size_t blkSize, size_t numBlocks, size_t alignment=0);
	void init();

	void resize(size_t numBlks);
	void* allocMem(const size_t numBlocks=1, const size_t alignment = 0);
	size_t freeMem(void* ptr);

	void reset();
	
	inline size_t getBlkSize() const noexcept { return blkSize; }

	inline size_t getNumUsedBlks() const noexcept {return numUsedBlocks; }
	inline size_t getNumBlks() const noexcept {return numBlks; }

	template<typename T=void>
	inline T* get(size_t pos) { return (T*)((char*)beginPtr + blkSize*pos); }

	template<typename T =void>
	inline const T* get(size_t pos) const { (T*)((char*)beginPtr + blkSize*pos);}

	inline size_t getPos(void* blk)
	{
		assert((uintptr_t)blk >= (uintptr_t)beginPtr && (uintptr_t)blk <= (uintptr_t)beginPtr + numBlks * blkSize);
		return ((uintptr_t)blk - (uintptr_t)beginPtr) / blkSize;
	}

	inline void* data(){	return beginPtr; }

	inline const void* data() const { return beginPtr; }

private:
	size_t blkSize;
	size_t numBlks;
	size_t alignment;
	size_t numUsedBlocks;
	void* beginPtr;
};


}
