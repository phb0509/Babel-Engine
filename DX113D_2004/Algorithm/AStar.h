#pragma once

class AStar
{
private:
	UINT width, height;

	vector<Node*> nodes;
	Heap* heap;

	Float2 interval;

	vector<Collider*> obstacles;
public:
	AStar(UINT width = 20, UINT height = 20);
	~AStar();

	void Update();
	void Render();

	void SetNode(class Terrain* terrain);
	void SetObstacle(vector<Collider*> value);

	int FindCloseNode(Vector3 pos);
	Vector3 FindCloseNodePosition(Vector3 pos);
	vector<Vector3> FindPath(int start, int end);
	void MakeDirectPath(IN Vector3 start, IN Vector3 end, OUT vector<Vector3>& path);

	bool CollisionObstacle(Ray ray, float destDistance);

	void Reset();
private:
	float GetDistance(int curIndex, int end);

	void Extend(int center, int end);
	int GetMinNode();
};