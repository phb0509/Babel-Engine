#include "Framework.h"

Heap::Heap()
{
}

Heap::~Heap()
{
}

void Heap::Insert(Node* node)
{
    heap.emplace_back(node);
    UpdateUpper(heap.size() - 1);
}

void Heap::UpdateUpper(int index)
{
    int curIndex = index;
    int parent = (curIndex - 1) / 2;

    while (curIndex != 0)
    {
        if (heap[parent]->f < heap[curIndex]->f)
            break;

        swap(heap[curIndex], heap[parent]);

        curIndex = parent;
        parent = (curIndex - 1) / 2;
    }
}

Node* Heap::DeleteRoot()
{
    Node* root = heap.front();

    swap(heap[0], heap.back());

    heap.pop_back();
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
        if (lChild < heap.size() && heap[lChild]->f < heap[minNode]->f)
            minNode = lChild;

        if (rChild < heap.size() && heap[rChild]->f < heap[minNode]->f)
            minNode = rChild;

        if (minNode == curIndex)
            break;

        swap(heap[curIndex], heap[minNode]);

        curIndex = minNode;
        lChild = curIndex * 2 + 1;
        rChild = lChild + 1;
    }
}

void Heap::Clear()
{
    heap.clear();
}
