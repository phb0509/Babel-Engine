#pragma once

class Node
{
public:
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
	}mState;

	Node(Vector3 pos, int index, Float2 interval);
	~Node();

	void Render();
	void AddEdge(Node* node);
	Collider* MakeObstacle();
	Collider* GetCollider() { return mCollider; };

	void SetTestNode(bool isCheck) { mbIsCheck = isCheck; };


private:
	Collider* mCollider;
	Collider* mObstacle;
	bool mbIsCheck;
	Vector3 mPosition;
	int mIndex;
	int mVia;
	float mF;
	float mG;
	float mH;
	Float2 mDistanceBetweenNodes;
	vector<EdgeInfo*> edges;
};