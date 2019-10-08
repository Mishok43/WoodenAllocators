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

	template<typename T>
	inline void resize(size_t newNumBlocks)
	{
		size_t tmpSizeTotal = newNumBlocks * blkSize;
		void* tmpBeginPtr = alignedChunkAlloc(alignment, tmpSizeTotal);
		for (uint32_t i = 0; i < numBlks; i++)
		{
			void* m = reinterpret_cast<void*>((char*)tmpBeginPtr + blkSize * i);
			new (m)T(std::move(((T*)beginPtr)[i]));
		}
		//std::memcpy(tmpBeginPtr, beginPtr, sizeTotal);
		alignedChunkFree(beginPtr);

		beginPtr = tmpBeginPtr;
		numBlks = newNumBlocks;
		sizeTotal = tmpSizeTotal;
	}

	template<typename T=char>
	inline T* allocMem(const size_t numAllocatingBlocks =1, const size_t alignment = 0)
	{
		if (numUsedBlocks + numAllocatingBlocks > numBlks)
		{
			resize<T>(numBlks * 2);
		}

		if (numAllocatingBlocks == 1)
		{
			return reinterpret_cast<T*>((char*)beginPtr + blkSize * (numUsedBlocks++));
		}
	}

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

	size_t getAlignment() const
	{
		return alignment;
	}
private:
	size_t blkSize;
	size_t numBlks;
	size_t alignment;
	size_t numUsedBlocks;
	void* beginPtr;
};


}
