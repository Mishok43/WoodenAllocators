#pragma once
#include "pch.h"
#include "WoodenAllocators/Allocator.h"
#include "WoodenAllocators/AllocatorPoolSwap.h"
#include <vector>

using namespace wal;

namespace
{
	TEST(AllocAligned, Init)
	{
		//AllocatorAligned<int> t;
		std::vector<int, AllocatorAligned<int>>* d = new std::vector<int, AllocatorAligned<int>>();
		d->resize(10);
		(*d)[0] = 5;
		(*d)[9] = 10;
	}

	TEST(AllocPoolSwap, Init)
	{
		struct A
		{
			float a, b, c;
		};

		AllocatorPoolSwap alloc;
		alloc.init(sizeof(A), 1024, sizeof(A));



		A* p0 = (A*)alloc.allocMem();
		ASSERT_EQ((uintptr_t)(p0) % sizeof(A), 0);

		p0->a = 10.0f;
		p0->b = -11.0f;

		alloc.freeMem(p0);

		p0 = (A*)alloc.allocMem();
		

	}


}

