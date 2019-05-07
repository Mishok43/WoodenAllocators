#include "pch.h"
#include "AllocatorFreeListTree.h"

namespace wal{
AllocatorFreeListTree::FreeHeaderData AllocatorFreeListTree::NIL;

AllocatorFreeListTree::AllocatorFreeListTree(const uint32_t totalSize) :
	Allocator(totalSize)
{
	init();
}


void AllocatorFreeListTree::init()
{
	beginPtr = (FreeHeaderData*)malloc(sizeTotal);
	reset();
}


AllocatorFreeListTree::FreeHeaderData* 
AllocatorFreeListTree::findBestFitFreeBlk(size_t blkSize) const
{
#if ALLOCATING_DEBUG
	assert(root != nullptr);
#endif
	FreeHeaderData* cur = root;
	FreeHeaderData* bestFit = &NIL;
	do 
	{
		uint32_t curBlkSize = getBlkSize(cur);
		if (blkSize > curBlkSize)
		{
			cur = cur->right;
		}
		else
		{
			if (blkSize < curBlkSize)
			{
				bestFit = cur;
				cur = cur->left;
			}
			else
			{
				bestFit = cur;
				break;
			}
		}
	} while (!isNIL(cur));

	return bestFit;
}

AllocatorFreeListTree::FreeHeaderData*
AllocatorFreeListTree::minFreeBlk(FreeHeaderData* node) const
{
#if ALLOCATING_DEBUG
	assert(node != nullptr && node != &NIL);
#endif

	FreeHeaderData* minNode = node;
	while (!isNIL(minNode) && !isNIL(minNode->left))
	{
		minNode = minNode->left;
	}

	return minNode;
}

AllocatorFreeListTree::FreeHeaderData*
AllocatorFreeListTree::maxFreeBlk(FreeHeaderData* node) const
{
#if ALLOCATING_DEBUG
	assert(node != nullptr && node != &NIL);
#endif

	FreeHeaderData* maxNode = node;
	while (!isNIL(maxNode) && !isNIL(maxNode->right))
	{
		maxNode = maxNode->right;
	}

	return maxNode;
}

AllocatorFreeListTree::FreeHeaderData*
AllocatorFreeListTree::successorFreeBlk(FreeHeaderData* node) const
{
#if ALLOCATING_DEBUG
	assert(node != nullptr && node != &NIL);
#endif

	if (!isNIL(node->right))
	{
		return minFreeBlk(node->right);
	}
	else
	{
		FreeHeaderData* prevNode = node;
		FreeHeaderData* successorNode = node->parent;
		while (!isNIL(successorNode) && successorNode->right == prevNode)
		{
			prevNode = successorNode;
			successorNode = successorNode->parent;
		}
		return successorNode;
	}
}

AllocatorFreeListTree::FreeHeaderData*
AllocatorFreeListTree::predecessorFreeBlk(FreeHeaderData* node) const
{
#if ALLOCATING_DEBUG
	assert(node != nullptr);
#endif


	if (!isNIL(node->left))
	{
		return maxFreeBlk(node->left);
	}
	else
	{
		FreeHeaderData* prevNode = node;
		FreeHeaderData* predeccessorNode = node->parent;
		while (!isNIL(predeccessorNode) && predeccessorNode->left == prevNode)
		{
			prevNode = predeccessorNode;
			predeccessorNode = predeccessorNode->left;
		}
		return predeccessorNode;
	}
}

inline uint32_t AllocatorFreeListTree::getBlkSize(void* blk) const
{
	return (uintptr_t)(((AllocatedHeaderData*)blk)->next) - (uintptr_t)blk;
}


void AllocatorFreeListTree::insertFreeBlk(FreeHeaderData* data)
{
#if ALLOCATING_DEBUG
	assert(data != nullptr);
#endif
	uint32_t blkSize = getBlkSize(data);
	FreeHeaderData* cur = root;
	bool isRightBlk;
	if (!isNIL(cur))
	{
		while (true)
		{
			isRightBlk = blkSize > getBlkSize(cur);
			if (isRightBlk)
			{
				if (!isNIL(cur->right))
				{
					cur = cur->right;
				}
			}
			else
			{
				if (!isNIL(cur->left))
				{
					cur = cur->left;
				}
			}
		}
	}

	data->right = &NIL;
	data->left = &NIL;
	data->bRed = true;
	if (!isNIL(cur))
	{
		data->parent = cur;
		if (isRightBlk)
		{
			cur->right = data;
		}
		else
		{
			cur->left = data;
		}
	}
	else
	{
		data->parent = &NIL;
		root = data;
	}

	insertFixUpTree(data);
}

void AllocatorFreeListTree::rotateRightFreeBlk(FreeHeaderData* y)
{
	FreeHeaderData* x = y->left;
	y->left = x->right;
	if (!isNIL(x->right))
	{
		x->right->parent = y;
	}

	x->parent = y->parent;
	if (isNIL(y->parent))
	{
		root = y;
	}
	else
	{
		if (y->parent->left == y)
		{
			y->parent->left = x;
		}
		else
		{
			y->parent->right = x;
		}
	}

	x->right = y;
	y->parent = x;
}

void AllocatorFreeListTree::rotateLeftFreeBlk(FreeHeaderData* x)
{
	FreeHeaderData* y = x->right;
	x->right = y->left;
	if (!isNIL(y->left))
	{
		y->left->parent = x;
	}
	y->parent = x->parent;
	if (isNIL(x->parent))
	{
		root = y;
	}
	else
	{
		if (x->parent->left == x)
		{
			x->parent->left = y;
		}
		else
		{
			x->parent->right = y;
		}
	}

	y->left = x;
	x->parent = y;
}


void AllocatorFreeListTree::insertFixUpTree(FreeHeaderData* z)
{
	while (z->parent->bRed)
	{
		FreeHeaderData* y;
		bool parentIsRightChild = z->parent == z->parent->parent->right;
		if (!parentIsRightChild)
		{
			y = z->parent->parent->right;
			if (y->bRed)
			{
				y->bRed = false;
				z->parent = false;
				z->parent->parent->bRed = true;
				z = z->parent->parent;
			}
			else
			{
				bool isRightChild = z == z->parent->right;
				if (isRightChild)
				{
					z = z->parent;
					rotateLeftFreeBlk(z);
				}

				z->parent->bRed = false;
				z->parent->parent->bRed = true;
				rotateRightFreeBlk(z->parent->parent);
			}
		}
		else{
			y = z->parent->parent->left;
			if (y->bRed)
			{
				y->bRed = false;
				z->parent = false;
				z->parent->parent->bRed = true;
				z = z->parent->parent;
			}
			else
			{
				bool isLeftChild = z == z->parent->left;
				if (isLeftChild)
				{
					z = z->parent;
					rotateRightFreeBlk(z);
				}

				z->parent->bRed = false;
				z->parent->parent->bRed = true;
				rotateLeftFreeBlk(z->parent->parent);
			}
		}
	}


	root->bRed = false;
}


void AllocatorFreeListTree::deleteFixUpTree(FreeHeaderData* x)
{
	while (root != x && !x->bRed)
	{
		FreeHeaderData* w;
		if (x == x->parent->left)
		{
			w = x->parent->right;
			if (w->bRed)
			{
				w->bRed = false;
				x->parent->bRed = true;
				rotateLeftFreeBlk(x->parent);
				w = x->parent->right;
			}
			if (!w->bRed && !w->right->bRed)
			{
				w->bRed = true;
				x = x->parent;
			}
			else
			{
				if (w->left->bRed)
				{
					w->bRed = true;
					w->left->bRed = false;
					rotateRightFreeBlk(w);
					w = x->parent->right;
				}

				w->bRed = x->parent->bRed;
				w->right->bRed = false;
				x->parent->bRed = false;
				rotateLeftFreeBlk(x->parent);
				x = root;
			}
		}
		else
		{
			w = x->parent->left;
			if (w->bRed)
			{
				w->bRed = false;
				x->parent->bRed = true;
				rotateRightFreeBlk(x->parent);
				w = x->parent->left;
			}
			if (!w->bRed && !w->left->bRed)
			{
				w->bRed = true;
				x = x->parent;
			}
			else
			{
				if (w->right->bRed)
				{
					w->bRed = true;
					w->right->bRed = false;
					rotateLeftFreeBlk(w);
					w = x->parent->left;
				}

				w->bRed = x->parent->bRed;
				w->left->bRed = false;
				x->parent->bRed = false;
				rotateRightFreeBlk(x->parent);
				x = root;
			}
		}
	}

	x->bRed = false;
}

void AllocatorFreeListTree::deleteFreeBlk(FreeHeaderData* z)
{
#if ALLOCATING_DEBUG
	assert(z != nullptr && !isNIL(z));
#endif
	bool isRoot = root == z;
	bool isRightChild;

	FreeHeaderData* parent = z->parent;
	if (!isRoot)
	{
#if ALLOCATING_DEBUG
		assert(!isNIL(parent));
#endif
		isRightChild = parent->right == z;
	}

	FreeHeaderData* x;
	FreeHeaderData* y = z;
	bool yIsRed = y->bRed;

	if (isNIL(z->left))
	{
		x = z->right;
		transitFreeBlk(z, z->right);
	}
	else
	{
		if (isNIL(z->right))
		{
			x = z->left;
			transitFreeBlk(z, z->left);
		}
		else
		{
			y = minFreeBlk(z->right);
			yIsRed = y->bRed;
			x = y->right;
			if (y->parent == z)
			{
				x->parent = y;
			}
			else
			{
				transitFreeBlk(y, y->right);
				y->right = z->right;
				z->right->parent = y;
			}

			transitFreeBlk(z, y);
			y->left = z->left;
			y->left->parent = y;
			y->bRed = z->bRed;
		}
	}

	if (!yIsRed)
	{
		deleteFixUpTree(x);
	}
}

void AllocatorFreeListTree::transitFreeBlk(FreeHeaderData* u, FreeHeaderData* v)
{
#if ALLOCATING_DEBUG
	assert(u->right == v || u->left == v);
#endif

	if (!isNIL(u->parent))
	{
		if (u->parent->right == u)
		{
			u->parent->right = v;
		}
		else
		{
			u->parent->left = v;
		}
	}
	else
	{
		root = v;
	}

	v->parent = u->parent;
}

void* AllocatorFreeListTree::allocMem(const size_t blkSize/* =1 */)
{
	uint32_t neededSize;
#if ALLOCATING_DEBUG
	assert(blkSize <= sizeTotal);
#endif
	neededSize = (uint32_t)blkSize + ALLOCATED_HEADER_DATA_SIZE;

	FreeHeaderData* freeBlk =  findBestFitFreeBlk(neededSize);
	if (isNIL(freeBlk))
		return nullptr;

	uint32_t freeBlkSize = getBlkSize(freeBlk);
	AllocatedHeaderData* res;
	 
	// if it's not possible to divide block to 2 blocks, because of unsufficient size, just use it fully. 
	if (freeBlkSize - neededSize < FREE_HEADER_DATA_SIZE)
	{
		res = (AllocatedHeaderData*)freeBlk;
		deleteFreeBlk(freeBlk);
	}
	else
	{
		deleteFreeBlk(freeBlk);
		
		res = (AllocatedHeaderData*)((char*)freeBlk + blkSize - neededSize);
		res->next = freeBlk->next;
		res->prev = freeBlk;

		freeBlk->next = (FreeHeaderData*)res;
		insertFreeBlk(freeBlk);
	}
	res->bAllocated = true;

	return getAfterAllocatedHeaderPtr(res);
}


void AllocatorFreeListTree::freeMem(void* ptr)
{
	AllocatedHeaderData* curBlk = (AllocatedHeaderData*)((char*)ptr - ALLOCATED_HEADER_DATA_SIZE);
#if ALLOCATING_DEBUG
	assert(!hasOnlyDebugValue(curBlk, ALLOCATED_HEADER_DATA_SIZE));
#endif

#if ALLOCATING_DEBUG
	assert(!hasOnlyDebugValue(getAfterAllocatedHeaderPtr(curBlk), getBlkSize(curBlk)-ALLOCATED_HEADER_DATA_SIZE));
#endif

	// Merge prev and cur free blocks if it's possible
	if (!curBlk->prev->bAllocated)
	{
		deleteFreeBlk(curBlk->prev);

		curBlk->prev->next = curBlk->next;
#if ALLOCATING_DEBUG
		setDebugValue((char*)curBlk, getBlkSize(curBlk));
#endif
		curBlk = curBlk->prev;
	}
	else
	{
		curBlk->bAllocated = false;
#if ALLOCATING_DEBUG
		setDebugValue(getAfterFreeHeaderPtr(curBlk), getBlkSize(curBlk)-FREE_HEADER_DATA_SIZE);
#endif
	}

	// Merge cur and next free blocks if it's possible
	if (!curBlk->next->bAllocated)
	{
		deleteFreeBlk(curBlk->next);

		curBlk->next = curBlk->next->next;
#if ALLOCATING_DEBUG
		setDebugValue(curBlk->next, getBlkSize(curBlk->next));
#endif
	}
	
	insertFreeBlk((FreeHeaderData*)curBlk);
}



void AllocatorFreeListTree::reset()
{
#if ALLOCATING_DEBUG
	setDebugValue(beginPtr, sizeTotal);
#endif

	root = &NIL;
	insertFreeBlk(beginPtr);
}


void* AllocatorFreeListTree::getAfterAllocatedHeaderPtr(void* ptr) const
{
	return (char*)ptr + ALLOCATED_HEADER_DATA_SIZE;
}

void* AllocatorFreeListTree::getAfterFreeHeaderPtr(void* ptr) const
{
	return (char*)ptr + FREE_HEADER_DATA_SIZE;
}

AllocatorFreeListTree::~AllocatorFreeListTree()
{
#if ALLOCATING_DEBUG
	assert(getBlkSize(beginPtr) == sizeTotal && "Memory leak");
#endif

	freeMem(beginPtr);
}

inline bool AllocatorFreeListTree::isNIL(FreeHeaderData* blk) const noexcept
{
	return blk == &NIL;
}

}