#include "Framework.h"

AStar::AStar(UINT width, UINT height)
	: width(width), height(height)
{
	heap = new Heap();
}

AStar::~AStar()
{
	for (Node* node : nodes)
		delete node;

	delete heap;
}

void AStar::Update()
{
	if (KEY_DOWN(VK_LBUTTON))
	{
		Ray ray = CAMERA->ScreenPointToRay(MOUSEPOS);

		for (Node* node : nodes)
		{
			if (node->collider->RayCollision(ray))
			{
				obstacles.emplace_back(node->MakeObstacle());
				break;
			}
		}
	}
}

void AStar::Render()
{
	for (Node* node : nodes)
		node->Render();	
}

void AStar::SetNode(Terrain* terrain) // 터레인에 노드셋팅. 개수설정할 수 있음.
{
	Float2 size = terrain->GetSize();

	interval.x = size.x / width;
	interval.y = size.y / height;

	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			Vector3 pos = Vector3(x * interval.x, 0, z * interval.y);
			pos.y = terrain->GetHeight(pos);

			int index = z * width + x;
			nodes.emplace_back(new Node(pos, index, interval));
		}
	}

	for (UINT i = 0; i < nodes.size(); i++)
	{
		if (i % width != width - 1)
		{
			nodes[i + 0]->AddEdge(nodes[i + 1]);
			nodes[i + 1]->AddEdge(nodes[i + 0]);
		}

		if (i < nodes.size() - width)
		{
			nodes[i + 0]->AddEdge(nodes[i + width]);
			nodes[i + width]->AddEdge(nodes[i + 0]);
		}

		if (i < nodes.size() - width && i % width != width - 1)
		{
			nodes[i + 0]->AddEdge(nodes[i + width + 1]);
			nodes[i + width + 1]->AddEdge(nodes[i + 0]);
		}

		if (i < nodes.size() - width && i % width != 0)
		{
			nodes[i + 0]->AddEdge(nodes[i + width - 1]);
			nodes[i + width - 1]->AddEdge(nodes[i + 0]);
		}
	}
}

void AStar::SetObstacle(vector<Collider*> value)
{
	for (Collider* obstacle : value)
	{
		for (Node* node : nodes)
		{
			node->collider->UpdateWorld();
			if (obstacle->Collision(node->collider))
			{
				node->state = Node::OBSTACLE;
			}
		}

		obstacles.emplace_back(obstacle);
	}
}

void AStar::SetDirectNode(int index)
{
	nodes[index]->state = Node::DIRECT;
}

int AStar::FindCloseNode(Vector3 pos) // 매개변수 pos와 가장 가까운 노드 인덱스 반환.
{
	float minDistance = FLT_MAX;
	int index = -1;

	for (UINT i = 0; i < nodes.size(); i++)
	{
		if (nodes[i]->state == Node::OBSTACLE)
			continue;

		float distance = Distance(pos, nodes[i]->pos);

		if (distance < minDistance)
		{
			minDistance = distance;
			index = i;
		}
	}

	return index;
}

Vector3 AStar::FindCloseNodePosition(Vector3 pos)
{
	float minDistance = FLT_MAX;
	int index = -1;

	for (UINT i = 0; i < nodes.size(); i++)
	{
		if (nodes[i]->state == Node::OBSTACLE)
			continue;

		float distance = Distance(pos, nodes[i]->pos);

		if (distance < minDistance)
		{
			minDistance = distance;
			index = i;
		}
	}

	return nodes[index]->pos;
}

vector<Vector3> AStar::FindPath(int start, int end)
{
	Reset();

	float G = 0.0f;
	float H = GetDistance(start, end);

	nodes[start]->g = G;
	nodes[start]->h = H;
	nodes[start]->f = G + H;
	nodes[start]->via = start;
	nodes[start]->state = Node::OPEN;

	heap->Insert(nodes[start]);

	while (nodes[end]->state != Node::CLOSED)
	{
		int curIndex = GetMinNode(); // F값 가장 적은거.
		Extend(curIndex, end);
		nodes[curIndex]->state = Node::CLOSED;
	}

	vector<Vector3> path;

	int curIndex = end;

	while (curIndex != start)
	{
		nodes[curIndex]->state = Node::USING;
		path.emplace_back(nodes[curIndex]->pos);
		curIndex = nodes[curIndex]->via;
	}

	nodes[curIndex]->state = Node::USING;
	path.emplace_back(nodes[curIndex]->pos);

	heap->Clear();

	return path;
}


void AStar::MakeDirectPath(IN Vector3 start, IN Vector3 end, OUT vector<Vector3>& path)
{
	//캐릭터 위치값, 목적지, 경로벡터(
	int cutNodeNum = 0;
	Ray ray;
	ray.position = start;

	for (UINT i = 0; i < path.size(); i++) // 목적지 -> 시작지.
	{
		ray.direction = path[i] - ray.position; // 캐릭터에서 path[i]까지 벡터
		float distance = ray.direction.Length(); // path[i]까지 거리.

		ray.direction.Normalize();


		// ray.direction : 캐릭터->path[i]까지 방향벡터.
		// ray.position = start
		// distance : 캐릭터->path[i]까지 거리..

		if (!CollisionObstacle(ray, distance)) // 캐릭터와 path[i]사이에 장애물이 없다면...!
		{
			cutNodeNum = path.size() - i - 1;  // 9
			break;
		}
	}

	for (int i = 0; i < cutNodeNum; i++) // 캐릭터에서 다이렉트로 갈수있는 곳까지 전부 빼버리기.
		path.pop_back();

	cutNodeNum = 0;
	ray.position = end;

	for (UINT i = 0; i < path.size(); i++) // size는 5
	{
		ray.direction = path[path.size() - i - 1] - ray.position; // 4 -> 0
		float distance = ray.direction.Length();

		ray.direction.Normalize();

		if (!CollisionObstacle(ray, distance)) // 충돌 안하면
		{
			cutNodeNum = path.size() - i - 1;
			break;
		}
	}

	for (int i = 0; i < cutNodeNum; i++)
		path.erase(path.begin());
}

bool AStar::CollisionObstacle(Ray ray, float destDistance)
{
	for (Collider* obstacle : obstacles)
	{
		Contact contact;

		if (obstacle->RayCollision(ray, &contact)) // ray방향에 장애물이 있다면
		{
			if (contact.distance < destDistance)
				return true;
		}
	}

	return false;
}

void AStar::Reset() // 장애물 제외 전부 NONE
{
	for (Node* node : nodes)
	{
		if (node->state != Node::OBSTACLE)
			node->state = Node::NONE;
	}
}

float AStar::GetDistance(int curIndex, int end)
{
	Vector3 startPos = nodes[curIndex]->pos;
	Vector3 endPos = nodes[end]->pos;

	return Distance(startPos, endPos);
}

void AStar::Extend(int center, int end)
{
	vector<Node::EdgeInfo*> edges = nodes[center]->edges;

	for (UINT i = 0; i < edges.size(); i++)
	{
		int index = edges[i]->index;

		if (nodes[index]->state == Node::CLOSED ||   //OPEN 이여야 건너뜀.
			nodes[index]->state == Node::OBSTACLE)
			continue;

		float G = nodes[center]->g + edges[i]->edgeCost;
		float H = GetDistance(index, end);
		float F = G + H;

		if (nodes[index]->state == Node::OPEN)
		{
			if (F < nodes[index]->f)
			{
				nodes[index]->g = G;
				nodes[index]->f = F;
				nodes[index]->via = center;
			}
		}
		else if (nodes[index]->state == Node::NONE)
		{
			nodes[index]->g = G;
			nodes[index]->h = H;
			nodes[index]->f = F;
			nodes[index]->via = center;
			nodes[index]->state = Node::OPEN;

			heap->Insert(nodes[index]);
		}
	}
}

int AStar::GetMinNode()
{
	return heap->DeleteRoot()->index;
}
