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
		if (blkSize > cur->size)
		{
			cur = cur->right;
		}
		else
		{
			if (blkSize < cur->size)
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

	if (node->right == nullptr || node->left == nullptr)
	{
		FreeHeaderData* nodeToReplace;
		if (node->left == nullptr && node->right == nullptr)
		{
			nodeToReplace = nullptr;
		}
		else
		{
			nodeToReplace = (node->left = nullptr) ? node->right : node->left;
		}

		if (!isRoot)
		{
			nodeToReplace->parent = parent;

			if (isRightChild)
			{
				parent->right = nodeToReplace;
			}
			else
			{
				parent->left = nodeToReplace;
			}
		}
		else
		{
			nodeToReplace->parent = nullptr;
			root = nodeToReplace;
		}
	}
	else
	{
		if (isRightChild)
		{
			FreeHeaderData* successor = successorFreeBlk(node);

		}
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

	FreeHeaderData* blk =  findBestFitFreeBlk(neededSize);
	AllocatedHeaderData* res;

	// if it's not possible to divide block to 2 blocks, because of unsuffiecent size, just use it fully. 
	if (blk->size - neededSize < FREE_HEADER_DATA_SIZE)
	{
		res = (AllocatedHeaderData*)blk;
		// TODO: rebalance RBTree
		blk->prev->next = blk->next;
		res->size = blk->size;
	}
	else
	{
		blk->size -= neededSize;
		res = (AllocatedHeaderData*)((char*)blk + blk->size - neededSize);
		res->size = neededSize;
		res->prev = blk;
		res->next = blk->next;
	}
	return curPtr + ALLOCATED_HEADER_DATA_SIZE;
}


void AllocatorFreeListTree::free(void* ptr)
{
	AllocatedHeaderData* curPtr = (AllocatedHeaderData*)((char*)ptr - ALLOCATED_HEADER_DATA_SIZE);
#if ALLOCATING_DEBUG
	assert(!hasOnlyDebugValue(curPtr, ALLOCATED_HEADER_DATA_SIZE));
#endif

	FreeHeaderData* prevFreePtr = beginPtr;
	while (prevFreePtr->next != 0)
	{
		char* nextFreePtr = (char*)beginPtr + prevFreePtr->next;
		if ((uintptr_t)nextFreePtr < (uintptr_t)curPtr)
		{
			prevFreePtr = (FreeHeaderData*)nextFreePtr;
		}
		else
		{
			break;
		}
	}

#if ALLOCATING_DEBUG
	assert(!hasOnlyDebugValue((char*)curPtr + ALLOCATED_HEADER_DATA_SIZE, curPtr->size));
#endif

	// Merge prev and cur free blocks if it's possible
	if ((char*)prevFreePtr + prevFreePtr->size == curPtr)
	{
		prevFreePtr->size += curPtr->size;
		prevFreePtr->next = curPtr->next;
#if ALLOCATING_DEBUG
		setDebugValue((char*)curPtr, curPtr->size + ALLOCATED_HEADER_DATA_SIZE);
#endif
		curPtr = prevFreePtr;
	}
	else
	{
		FreeHeaderData* curFreePtr = (FreeHeaderData*)curPtr;
		curFreePtr->next = prevFreePtr->next;
		prevFreePtr->next = (uintptr_t)curFreePtr - (uintptr_t)beginPtr;
#if ALLOCATING_DEBUG
		setDebugValue((char*)curPtr + ALLOCATED_HEADER_DATA_SIZE, curPtr->size);
#endif
	}

	// Merge cur and next free blocks if it's possible
	if (curPtr->nextOffset != 0)
	{
		FreeHeaderData* nextFreePtr = (char*)beginPtr + curPtr->nextOffset;
		if ((char*)curPtr + curPtr->size == nextFreePtr)
		{
			curPtr->size += nextFreePtr->size;
			curPtr->nextOffset = nextFreePtr->nextOffset;
		}
	}
}



void AllocatorFreeListTree::reset()
{
#if ALLOCATING_DEBUG
	setDebugValue(beginPtr, sizeTotal);
#endif

	root = beginPtr;

	root->size = sizeTotal;
	root->prev = 0;
	root->next = 0;
	root->left = 0;
	root->right = 0;
}


AllocatorFreeListTree::~AllocatorFreeListTree()
{
#if ALLOCATING_DEBUG
	assert(beginPtr->size == sizeTotal && "Memory leak");
	assert(hasOnlyDebugValue((char*)beginPtr + FREE_HEADER_DATA_SIZE, beginPtr->size - FREE_HEADER_DATA_SIZE) && "Memory leak or ub");
#endif

	free(beginPtr);
}
