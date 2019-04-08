#pragma once

#include "Allocator.h"


template<bool safe=true>
class AllocatorStack : public Allocator
{
public:
	AllocatorStack(const std::size_t totalSize);

	void init() override;

	void* allocate(const std::size_t size, const std::size_t alignment = 0) override;
	void free(void* ptr) override;

	void reset() override;

private:
	void* beginPtr = nullptr;
	void* curPtr = nullptr;


	// TODO - use additional array containing the sizes of data
	void* data = 
};

