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
		uint32_t padding = ptr % alignment;
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

//
//template<typename T, uint8_t alignment = alignof(T)>
//class AllocatorAligned: public Allocator
//{
//public : 
//    //    typedefs
//    typedef T value_type;
//    typedef value_type* pointer;
//    typedef const value_type* const_pointer;
//    typedef value_type& reference;
//    typedef const value_type& const_reference;
//    typedef std::size_t size_type;
//    typedef std::ptrdiff_t difference_type;
//
//public : 
//    //    convert an allocator<T> to allocator<U>
//    template<typename U>
//    struct rebind {
//        typedef AllocatorAligned2<U> other;
//    };
//
//public : 
//    inline explicit AllocatorAligned() {}
//    inline ~AllocatorAligned() {}
//    inline explicit AllocatorAligned(AllocatorAligned const&) {}
//    template<typename U>
//    inline explicit AllocatorAligned(AllocatorAligned2<U> const&) {}
//
//    //    address
//    inline pointer address(reference r) { return &r; }
//    inline const_pointer address(const_reference r) { return &r; }
//
//    //    memory allocation
//    inline pointer allocate(size_type cnt, 
//       typename std::allocator<void>::const_pointer = 0) { 
//      return reinterpret_cast<pointer>( alignedChunkAlloc(alignment, cnt*sizeof(T)) );
//    }
//    inline void deallocate(pointer p, size_type) { 
//		p->~T();
//		alignedChunkFree(p);
//    }
//
//    //    size
//    inline size_type max_size() const { 
//        return std::numeric_limits<size_type>::max() / sizeof(T);
// }
//
//    //    construction/destruction
//    inline void construct(pointer p, const T& t) { new(p) T(t); }
//    inline void destroy(pointer p) { p->~T(); }
//
//    inline bool operator==(AllocatorAligned const&) { return true; }
//    inline bool operator!=(AllocatorAligned const& a) { return !operator==(a); }
//};

template <typename T, std::size_t Alignment=alignof(T) >
class AllocatorAligned2
{
public:

	// The following will be the same for virtually all allocators.
	typedef T * pointer;
	typedef const T * const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;
	typedef std::size_t size_type;
	typedef ptrdiff_t difference_type;

	T * address(T& r) const
	{
		return &r;
	}

	const T * address(const T& s) const
	{
		return &s;
	}

	std::size_t max_size() const
	{
		// The following has been carefully written to be independent of
		// the definition of size_t and to avoid signed/unsigned warnings.
		return (static_cast<std::size_t>(0) - static_cast<std::size_t>(1)) / sizeof(T);
	}


	// The following must be the same for all allocators.
	template <typename U>
	struct rebind
	{
		typedef AllocatorAligned2<U, Alignment> other;
	};

	bool operator!=(const AllocatorAligned2& other) const
	{
		return !(*this == other);
	}

	void construct(T * const p, const T& t) const
	{
		void * const pv = static_cast<void *>(p);

		new (pv) T(t);
	}

	void destroy(T * const p) const
	{
		p->~T();
	}

	// Returns true if and only if storage allocated from *this
	// can be deallocated from other, and vice versa.
	// Always returns true for stateless allocators.
	bool operator==(const AllocatorAligned2& other) const
	{
		return true;
	}


	// Default constructor, copy constructor, rebinding constructor, and destructor.
	// Empty for stateless allocators.
	AllocatorAligned2()
	{
	}

	AllocatorAligned2(const AllocatorAligned2&)
	{
	}

	template <typename U> AllocatorAligned2(const AllocatorAligned2<U, Alignment>&)
	{
	}

	~AllocatorAligned2()
	{
	}


	// The following will be different for each allocator.
	T * allocate(const std::size_t n) const
	{
		// The return value of allocate(0) is unspecified.
		// Mallocator returns NULL in order to avoid depending
		// on malloc(0)'s implementation-defined behavior
		// (the implementation can define malloc(0) to return NULL,
		// in which case the bad_alloc check below would fire).
		// All allocators can return NULL in this case.
		if (n == 0)
		{
			return NULL;
		}

		// All allocators should contain an integer overflow check.
		// The Standardization Committee recommends that std::length_error
		// be thrown in the case of integer overflow.
		if (n > max_size())
		{
			throw std::length_error("AllocatorAligned2<T>::allocate() - Integer overflow.");
		}

		// Mallocator wraps malloc().
		void * const pv = _mm_malloc(n * sizeof(T), Alignment);

		// Allocators should throw std::bad_alloc in the case of memory allocation failure.
		if (pv == NULL)
		{
			throw std::bad_alloc();
		}

		return static_cast<T *>(pv);
	}

	void deallocate(T * const p, const std::size_t n) const
	{
		_mm_free(p);
	}


	// The following will be the same for all allocators that ignore hints.
	template <typename U>
	T * allocate(const std::size_t n, const U * /* const hint */) const
	{
		return allocate(n);
	}


	// Allocators are not required to be assignable, so
	// all allocators should have a private unimplemented
	// assignment operator. Note that this will trigger the
	// off-by-default (enabled under /Wall) warning C4626
	// "assignment operator could not be generated because a
	// base class assignment operator is inaccessible" within
	// the STL headers, but that warning is useless.
private:
	AllocatorAligned2& operator=(const AllocatorAligned2&);
};

}