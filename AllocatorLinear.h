#pragma once

#include "Allocator.h"


class AllocatorLinear : public Allocator
{
public:
	AllocatorLinear(const size_t totalSize);
	~AllocatorLinear();

	void init() override;

	void* allocate(const size_t size, const size_t alignment = 0) override;
	void free(void* ptr) override;

	void reset() override;

private:
	void* beginPtr = nullptr;
	void* curPtr = nullptr;
};

