#include "pch.h"
#include "AllocatorFreeListTree.h"


AllocatorFreeListTree::AllocatorFreeListTree(const uint32_t totalSize) :
	Allocator(totalSize)
{
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
	FreeHeaderData* bestFit = nullptr;
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
	} while (cur);

	return bestFit;
}

AllocatorFreeListTree::FreeHeaderData*
AllocatorFreeListTree::minFreeBlk(FreeHeaderData* node) const
{
#if ALLOCATING_DEBUG
	assert(node != nullptr);
#endif

	FreeHeaderData* minNode = node;
	while (minNode && minNode->left)
	{
		minNode = minNode->left;
	}

	return minNode;
}

AllocatorFreeListTree::FreeHeaderData*
AllocatorFreeListTree::maxFreeBlk(FreeHeaderData* node) const
{
#if ALLOCATING_DEBUG
	assert(node != nullptr);
#endif

	FreeHeaderData* maxNode = node;
	while (maxNode && maxNode->right)
	{
		maxNode = maxNode->right;
	}

	return maxNode;
}

AllocatorFreeListTree::FreeHeaderData*
AllocatorFreeListTree::successorFreeBlk(FreeHeaderData* node) const
{
#if ALLOCATING_DEBUG
	assert(node != nullptr);
#endif

	if (node->right)
	{
		return minFreeBlk(node->right);
	}
	else
	{
		FreeHeaderData* prevNode = node;
		FreeHeaderData* successorNode = node->parent;
		while (successorNode && successorNode->right == prevNode)
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


	if (node->left)
	{
		return maxFreeBlk(node->left);
	}
	else
	{
		FreeHeaderData* prevNode = node;
		FreeHeaderData* predeccessorNode = node->parent;
		while (predeccessorNode && predeccessorNode->left == prevNode)
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
	while(true)
	{
		isRightBlk = blkSize > getBlkSize(cur);
		if (isRightBlk)
		{
			if (cur->right)
			{
				cur = cur->right;
			}
		}
		else
		{
			if (cur->left)
			{
				cur = cur->left;
			}
		}
	}

	root->right = nullptr;
	root->left = nullptr;
	if (cur != nullptr)
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
		root = cur;
		root->parent = nullptr;
	}

	//TODO: rebalance
}


void AllocatorFreeListTree::deleteFreeBlk(FreeHeaderData* node)
{
#if ALLOCATING_DEBUG
	assert(node != nullptr);
#endif
	bool isRoot = root == node;
	bool isRightChild;

	FreeHeaderData* parent = node->parent;
	if (!isRoot)
	{
#if ALLOCATING_DEBUG
		assert(parent != nullptr);
		isRightChild = parent->right == node;
#endif
	}

	FreeHeaderData* nodeToReplace;
	if (node->right == nullptr || node->left == nullptr)
	{
		if (node->left == nullptr && node->right == nullptr)
		{
			nodeToReplace = nullptr;
		}
		else
		{
			nodeToReplace = (node->left = nullptr) ? node->right : node->left;
		}
	}
	else
	{
		nodeToReplace = minFreeBlk(node->right);
		if (nodeToReplace->right != nullptr)
		{
			transitFreeBlk(nodeToReplace, nodeToReplace->right);
		}
	}

	transitFreeBlk(node, nodeToReplace);
}

void AllocatorFreeListTree::transitFreeBlk(FreeHeaderData* node, FreeHeaderData* child)
{
#if ALLOCATING_DEBUG
	assert(node->right == child || node->left == child);
#endif

	if (node->parent != nullptr)
	{
		child->parent = node->parent;
		if (node->parent->right == node)
		{
			node->parent->right = child;
		}
		else
		{
			node->parent->left = child;
		}
	}
	else
	{
		root = child;
		child->parent = nullptr;
	}
}

void* AllocatorFreeListTree::allocate(const size_t blkSize/* =1 */, const size_t alignment /* = 0 */)
{
	uint32_t neededSize;
#if ALLOCATING_DEBUG
	assert(blkSize <= sizeTotal);
	assert(alignment == 0, "Alignment not supported!");
#endif
	neededSize = (uint32_t)blkSize + ALLOCATED_HEADER_DATA_SIZE;

	FreeHeaderData* freeBlk =  findBestFitFreeBlk(neededSize);
	uint32_t freeBlkSize = getBlkSize(freeBlk);
	AllocatedHeaderData* res;
	 
	// if it's not possible to divide block to 2 blocks, because of unsufficient size, just use it fully. 
	if (freeBlkSize - neededSize < FREE_HEADER_DATA_SIZE)
	{
		res = (AllocatedHeaderData*)freeBlk;
	}
	else
	{
		res = (AllocatedHeaderData*)((char*)freeBlk + blkSize - neededSize);
		res->next = freeBlk->next;
		res->prev = freeBlk;

		freeBlk->next = (FreeHeaderData*)res;
	}
	res->bAllocated = true;

	return getAfterAllocatedHeaderPtr(res);
}


void AllocatorFreeListTree::free(void* ptr)
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
		curBlk->prev->next = curBlk->next;
#if ALLOCATING_DEBUG
		setDebugValue((char*)curBlk, getBlkSize(curBlk));
#endif
		curBlk = curBlk->prev;
	}
	else
	{
		curBlk->bAllocated = false;
		insertFreeBlk((FreeHeaderData*)curBlk);
#if ALLOCATING_DEBUG
		setDebugValue(getAfterFreeHeaderPtr(curBlk), getBlkSize(curBlk)-FREE_HEADER_DATA_SIZE);
#endif
	}

	// Merge cur and next free blocks if it's possible
	if (!curBlk->next->bAllocated)
	{
		curBlk->next = curBlk->next->next;
#if ALLOCATING_DEBUG
		setDebugValue(curBlk->next, getBlkSize(curBlk->next));
#endif
	}
}



void AllocatorFreeListTree::reset()
{
#if ALLOCATING_DEBUG
	setDebugValue(beginPtr, sizeTotal);
#endif

	root = beginPtr;
	root->prev = 0;
	root->next = 0;
	root->left = 0;
	root->right = 0;
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
	assert(hasOnlyDebugValue(getAfterFreeHeaderPtr(beginPtr), getBlkSize(beginPtr)- FREE_HEADER_DATA_SIZE) && "Memory leak or ub");
#endif

	free(beginPtr);
}
