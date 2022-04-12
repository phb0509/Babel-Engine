#pragma once

class AStar
{
public:
	AStar(UINT width = 20, UINT height = 20);
	~AStar();

	void Update();
	void Render();

	void SetNode(class Terrain* terrain);
	void SetObstacle(vector<Collider*> value);
	void SetDirectNode(int index);
	void SetTestNode(int index);

	int FindCloseNode(Vector3 pos);
	Vector3 FindCloseNodePosition(Vector3 pos);
	vector<Vector3> FindPath(int start, int end);
	void MakeDirectPath(IN Vector3 start, IN Vector3 end, OUT vector<Vector3>& path);

	bool CollisionObstacle(Ray ray, float destDistance);

	void Reset();

	void SetCheckFalse();

private:
	float getDistance(int curIndex, int end);
	void extend(int center, int end);
	int getMinNode();

private:
	UINT mWidth;
	UINT mHeight;
	vector<Node*> mNodes;
	Heap* mHeap;
	Float2 mInterval;
	vector<Collider*> mObstacles;
};