#include "Framework.h"

AStar::AStar(UINT width, UINT height)
	: mWidth(width), mHeight(height)
{
	mHeap = new Heap();
}

AStar::~AStar()
{
	for (Node* node : mNodes)
		delete node;

	delete mHeap;
}

void AStar::Update()
{
	/*if (KEY_DOWN(VK_LBUTTON))
	{
		Ray ray = TARGETCAMERA->ScreenPointToRay(MOUSEPOS);

		for (Node* node : mNodes)
		{
			if (node->collider->RayCollision(ray))
			{
				mObstacles.emplace_back(node->MakeObstacle());
				break;
			}
		}
	}*/
}

void AStar::Render()
{
	for (Node* node : mNodes)
		node->Render();	
}

void AStar::SetNode(Terrain* terrain) // �ͷ��ο� ������. ���������� �� ����.
{
	Float2 size = terrain->GetSize();

	mInterval.x = size.x / mWidth;
	mInterval.y = size.y / mHeight;

	for (UINT z = 0; z < mHeight; z++)
	{
		for (UINT x = 0; x < mWidth; x++)
		{
			Vector3 pos = Vector3(x * mInterval.x, 0, z * mInterval.y);
			pos.y = terrain->GetHeight(pos);

			int index = z * mWidth + x;
			mNodes.emplace_back(new Node(pos, index, mInterval));
		}
	}

	for (UINT i = 0; i < mNodes.size(); i++)
	{
		if (i % mWidth != mWidth - 1)
		{
			mNodes[i + 0]->AddEdge(mNodes[i + 1]);
			mNodes[i + 1]->AddEdge(mNodes[i + 0]);
		}

		if (i < mNodes.size() - mWidth)
		{
			mNodes[i + 0]->AddEdge(mNodes[i + mWidth]);
			mNodes[i + mWidth]->AddEdge(mNodes[i + 0]);
		}

		if (i < mNodes.size() - mWidth && i % mWidth != mWidth - 1)
		{
			mNodes[i + 0]->AddEdge(mNodes[i + mWidth + 1]);
			mNodes[i + mWidth + 1]->AddEdge(mNodes[i + 0]);
		}

		if (i < mNodes.size() - mWidth && i % mWidth != 0)
		{
			mNodes[i + 0]->AddEdge(mNodes[i + mWidth - 1]);
			mNodes[i + mWidth - 1]->AddEdge(mNodes[i + 0]);
		}
	}
}

void AStar::SetObstacle(vector<Collider*> value)
{
	for (Collider* obstacle : value)
	{
		for (Node* node : mNodes)
		{
			node->collider->UpdateWorld();
			if (obstacle->Collision(node->collider))
			{
				node->state = Node::OBSTACLE;
			}
		}

		mObstacles.emplace_back(obstacle);
	}
}

void AStar::SetDirectNode(int index)
{
	mNodes[index]->state = Node::DIRECT;
}

void AStar::SetTestNode(int index)
{
	mNodes[index]->mIsCheck = true;
}

int AStar::FindCloseNode(Vector3 pos) // �Ű����� pos�� ���� ����� ��� �ε��� ��ȯ.
{
	float minDistance = FLT_MAX;
	int index = -1;

	for (UINT i = 0; i < mNodes.size(); i++)
	{
		if (mNodes[i]->state == Node::OBSTACLE)
			continue;

		float distance = Distance(pos, mNodes[i]->pos);

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

	for (UINT i = 0; i < mNodes.size(); i++)
	{
		if (mNodes[i]->state == Node::OBSTACLE)
			continue;

		float distance = Distance(pos, mNodes[i]->pos);

		if (distance < minDistance)
		{
			minDistance = distance;
			index = i;
		}
	}

	return mNodes[index]->pos;
}

vector<Vector3> AStar::FindPath(int start, int end)
{
	Reset();

	float G = 0.0f;
	float H = getDistance(start, end);

	mNodes[start]->g = G;
	mNodes[start]->h = H;
	mNodes[start]->f = G + H;
	mNodes[start]->via = start;
	mNodes[start]->state = Node::OPEN;

	mHeap->Insert(mNodes[start]);

	while (mNodes[end]->state != Node::CLOSED)
	{
		int curIndex = getMinNode(); // F�� ���� ������.
		extend(curIndex, end);
		mNodes[curIndex]->state = Node::CLOSED;
	}

	vector<Vector3> path;

	int curIndex = end;

	while (curIndex != start)
	{
		mNodes[curIndex]->state = Node::USING;
		path.emplace_back(mNodes[curIndex]->pos);
		curIndex = mNodes[curIndex]->via;
	}

	mNodes[curIndex]->state = Node::USING;
	path.emplace_back(mNodes[curIndex]->pos);

	mHeap->Clear();

	return path;
}


void AStar::MakeDirectPath(IN Vector3 start, IN Vector3 end, OUT vector<Vector3>& path)
{
	//ĳ���� ��ġ��, ������, ��κ���(
	int cutNodeNum = 0;
	Ray ray;
	ray.position = start;

	for (UINT i = 0; i < path.size(); i++) // ������ -> ������.
	{
		ray.direction = path[i] - ray.position; // ĳ���Ϳ��� path[i]���� ����
		float distance = ray.direction.Length(); // path[i]���� �Ÿ�.

		ray.direction.Normalize();


		// ray.direction : ĳ����->path[i]���� ���⺤��.
		// ray.position = start
		// distance : ĳ����->path[i]���� �Ÿ�..

		if (!CollisionObstacle(ray, distance)) // ĳ���Ϳ� path[i]���̿� ��ֹ��� ���ٸ�...!
		{
			cutNodeNum = path.size() - i - 1;  // 9
			break;
		}
	}

	for (int i = 0; i < cutNodeNum; i++) // ĳ���Ϳ��� ���̷�Ʈ�� �����ִ� ������ ���� ��������.
		path.pop_back();

	cutNodeNum = 0;
	ray.position = end;

	for (UINT i = 0; i < path.size(); i++) // size�� 5
	{
		ray.direction = path[path.size() - i - 1] - ray.position; // 4 -> 0
		float distance = ray.direction.Length();

		ray.direction.Normalize();

		if (!CollisionObstacle(ray, distance)) // �浹 ���ϸ�
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
	for (Collider* obstacle : mObstacles)
	{
		Contact contact;

		if (obstacle->RayCollision(ray, &contact)) // ray���⿡ ��ֹ��� �ִٸ�
		{
			if (contact.distance < destDistance)
				return true;
		}
	}

	return false;
}

void AStar::Reset() // ��ֹ� ���� ���� NONE
{
	for (Node* node : mNodes)
	{
		if (node->state != Node::OBSTACLE)
			node->state = Node::NONE;
	}
}

void AStar::SetCheckFalse()
{
	for (int i = 0; i < mNodes.size(); i++)
	{
		mNodes[i]->SetTestNode(false);
	}
}

float AStar::getDistance(int curIndex, int end)
{
	Vector3 startPos = mNodes[curIndex]->pos;
	Vector3 endPos = mNodes[end]->pos;

	return Distance(startPos, endPos);
}

void AStar::extend(int center, int end)
{
	vector<Node::EdgeInfo*> edges = mNodes[center]->edges;

	for (UINT i = 0; i < edges.size(); i++)
	{
		int index = edges[i]->index;

		if (mNodes[index]->state == Node::CLOSED ||   //OPEN �̿��� �ǳʶ�.
			mNodes[index]->state == Node::OBSTACLE)
			continue;

		float G = mNodes[center]->g + edges[i]->edgeCost;
		float H = getDistance(index, end);
		float F = G + H;

		if (mNodes[index]->state == Node::OPEN)
		{
			if (F < mNodes[index]->f)
			{
				mNodes[index]->g = G;
				mNodes[index]->f = F;
				mNodes[index]->via = center;
			}
		}
		else if (mNodes[index]->state == Node::NONE)
		{
			mNodes[index]->g = G;
			mNodes[index]->h = H;
			mNodes[index]->f = F;
			mNodes[index]->via = center;
			mNodes[index]->state = Node::OPEN;

			mHeap->Insert(mNodes[index]);
		}
	}
}

int AStar::getMinNode()
{
	return mHeap->DeleteRoot()->index;
}
