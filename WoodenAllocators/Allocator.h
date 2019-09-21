#pragma once
#include "pch.h"
#include <memory_resource>

namespace wal{

// Not generate virtual table for Allocator class, because it's a interface class
class Allocator
{
protected:
	size_t sizeTotal;
	size_t sizeUsed;

public:
	Allocator();
	Allocator(const size_t totalSize);

	size_t getSize() const;

	inline static uint32_t computePadding(uintptr_t ptr, uint32_t alignment)
	{
		uint32_t padding = ptr & alignment;
		return (padding == 0) ? 0 : alignment - padding;
	}

	static void* alignedChunkAlloc(uint32_t alignment, uint32_t size);

	static void alignedChunkFree(void* ptr);
protected:

#if ALLOCATING_DEBUG
	static void setDebugValue(void* ptr, size_t numBytes);
	static bool hasOnlyDebugValue(void* ptr, size_t numBytes);
#endif
};


template<typename T, uint8_t alignment = alignof(T)>
class AllocatorAligned: public Allocator
{
public : 
    //    typedefs
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

public : 
    //    convert an allocator<T> to allocator<U>
    template<typename U>
    struct rebind {
        typedef AllocatorAligned<U> other;
    };

public : 
    inline explicit AllocatorAligned() {}
    inline ~AllocatorAligned() {}
    inline explicit AllocatorAligned(AllocatorAligned const&) {}
    template<typename U>
    inline explicit AllocatorAligned(AllocatorAligned<U> const&) {}

    //    address
    inline pointer address(reference r) { return &r; }
    inline const_pointer address(const_reference r) { return &r; }

    //    memory allocation
    inline pointer allocate(size_type cnt, 
       typename std::allocator<void>::const_pointer = 0) { 
      return reinterpret_cast<pointer>( alignedChunkAlloc(alignment, cnt*sizeof(T)) );
    }
    inline void deallocate(pointer p, size_type) { 
		p->~T();
		alignedChunkFree(p);
    }

    //    size
    inline size_type max_size() const { 
        return std::numeric_limits<size_type>::max() / sizeof(T);
 }

    //    construction/destruction
    inline void construct(pointer p, const T& t) { new(p) T(t); }
    inline void destroy(pointer p) { p->~T(); }

    inline bool operator==(AllocatorAligned const&) { return true; }
    inline bool operator!=(AllocatorAligned const& a) { return !operator==(a); }
};

}