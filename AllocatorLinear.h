#pragma once

#include "Allocator.h"

namespace wal{

class AllocatorLinear : public Allocator
{
public:
	AllocatorLinear(const size_t totalSize);
	~AllocatorLinear();

	void init();

	void* allocMem(const size_t size, const size_t alignment = 0);
	void freeMem(void* ptr);

	void reset();

private:
	void* beginPtr = nullptr;
	void* curPtr = nullptr;
};

}