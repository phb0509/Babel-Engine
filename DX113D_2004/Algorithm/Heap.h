#pragma once

class Heap
{

public:
	Heap();
	~Heap();

	void Insert(Node* node);
	void UpdateUpper(int index);

	Node* DeleteRoot();
	void UpdateLower(int index);

	void Clear();

private:
	vector<Node*> mHeap;
};