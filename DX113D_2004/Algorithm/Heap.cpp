#include "Framework.h"

Heap::Heap()
{
}

Heap::~Heap()
{
}

void Heap::Insert(Node* node)
{
	mHeap.emplace_back(node);
	UpdateUpper(mHeap.size() - 1);
}

void Heap::UpdateUpper(int index)
{
	int curIndex = index;
	int parent = (curIndex - 1) / 2;

	while (curIndex != 0)
	{
		if (mHeap[parent]->mF < mHeap[curIndex]->mF)
		{
			break;
		}

		swap(mHeap[curIndex], mHeap[parent]);

		curIndex = parent;
		parent = (curIndex - 1) / 2;
	}
}

Node* Heap::DeleteRoot()
{
	Node* root = mHeap.front();

	swap(mHeap[0], mHeap.back());

	mHeap.pop_back();
	UpdateLower(0);

	return root;
}

void Heap::UpdateLower(int index)
{
	int curIndex = index;
	int lChild = index * 2 + 1;
	int rChild = lChild + 1;
	int minNode = curIndex;

	while (true)
	{
		if (lChild < mHeap.size() && mHeap[lChild]->mF < mHeap[minNode]->mF)
		{
			minNode = lChild;
		}

		if (rChild < mHeap.size() && mHeap[rChild]->mF < mHeap[minNode]->mF)
		{
			minNode = rChild;
		}

		if (minNode == curIndex)
		{
			break;
		}

		swap(mHeap[curIndex], mHeap[minNode]);

		curIndex = minNode;
		lChild = curIndex * 2 + 1;
		rChild = lChild + 1;
	}
}

void Heap::Clear()
{
	mHeap.clear();
}
