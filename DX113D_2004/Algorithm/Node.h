#pragma once

class Node
{
private:
	friend class AStar;
	friend class Heap;

	struct EdgeInfo
	{
		int index;
		float edgeCost;
	};

	enum State
	{
		NONE,
		OPEN,
		CLOSED,
		USING,
		OBSTACLE,
		DIRECT,
		TEST
	}state;

	Collider* collider;	
	Collider* obstacle;

	Vector3 pos;
	int index;
	int via;

	float f, g, h;

	Float2 interval;

	vector<EdgeInfo*> edges;

	Node(Vector3 pos, int index, Float2 interval);
	~Node();

	void Render();

	void AddEdge(Node* node);

	Collider* MakeObstacle();

	bool mIsCheck = false;

public:
	Collider* GetCollider() { return collider; };
	void SetTestNode(bool isCheck) { mIsCheck = isCheck; };
};