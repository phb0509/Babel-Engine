#include "Framework.h"

AStar::AStar(UINT width, UINT height): 
	mWidth(width), 
	mHeight(height), // 터레인의 각 행,열에 배치할 노드개수.
	mInterval(0.0f,0.0f)
{
	mHeap = new Heap();
}

AStar::~AStar()
{
	for (auto node : mNodeMap)
	{
		GM->SafeDelete(node);
	}
		
	GM->SafeDelete(mHeap);
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
	/*for (Node* node : mNodeMap)
	{
		node->Render();
	}*/
}

void AStar::SetNodeToTerrain(Terrain* terrain) // 터레인에 노드셋팅. 개수설정할 수 있음.
{
	//Float2 size = terrain->GetSize();

	//mInterval.x = size.x / mWidth; // mWidth,mHeight 의 default값 20.
	//mInterval.y = size.y / mHeight;

	//for (UINT z = 0; z < mHeight; z++)
	//{
	//	for (UINT x = 0; x < mWidth; x++)
	//	{
	//		Vector3 pos = Vector3(x * mInterval.x, 0, z * mInterval.y);
	//		pos.y = terrain->GetHeight(pos);

	//		int index = z * mWidth + x;
	//		mNodeMap.emplace_back(new Node(pos, index, mInterval));
	//	}
	//}

	//for (UINT i = 0; i < mNodeMap.size(); i++)
	//{
	//	if (i % mWidth != mWidth - 1)
	//	{
	//		mNodeMap[i + 0]->AddEdge(mNodeMap[i + 1]);
	//		mNodeMap[i + 1]->AddEdge(mNodeMap[i + 0]);
	//	}

	//	if (i < mNodeMap.size() - mWidth)
	//	{
	//		mNodeMap[i + 0]->AddEdge(mNodeMap[i + mWidth]);
	//		mNodeMap[i + mWidth]->AddEdge(mNodeMap[i + 0]);
	//	}

	//	if (i < mNodeMap.size() - mWidth && i % mWidth != mWidth - 1)
	//	{
	//		mNodeMap[i + 0]->AddEdge(mNodeMap[i + mWidth + 1]);
	//		mNodeMap[i + mWidth + 1]->AddEdge(mNodeMap[i + 0]);
	//	}

	//	if (i < mNodeMap.size() - mWidth && i % mWidth != 0)
	//	{
	//		mNodeMap[i + 0]->AddEdge(mNodeMap[i + mWidth - 1]);
	//		mNodeMap[i + mWidth - 1]->AddEdge(mNodeMap[i + 0]);
	//	}
	//}
}

void AStar::SetObstacle(vector<Collider*> value) // Obstacle벡터 셋팅.
{
	for (Collider* obstacle : value)
	{
		for (Node* node : mNodeMap)
		{
			node->mCollider->UpdateWorld();

			if (obstacle->Collision(node->mCollider))
			{
				node->mState = Node::OBSTACLE;
			}
		}

		mObstacles.emplace_back(obstacle);
	}
}

void AStar::SetDirectNode(int index)
{
	mNodeMap[index]->mState = Node::DIRECT;
}

int AStar::FindCloseNode(Vector3 pos) // 매개변수 pos와 가장 가까운 노드 인덱스 반환.
{
	float minDistance = FLT_MAX;
	int index = -1;

	for (UINT i = 0; i < mNodeMap.size(); i++)
	{
		if (mNodeMap[i]->mState == Node::OBSTACLE)
			continue;

		float distance = Distance(pos, mNodeMap[i]->mPosition);

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

	for (UINT i = 0; i < mNodeMap.size(); i++)
	{
		if (mNodeMap[i]->mState == Node::OBSTACLE)
		{
			continue;
		}
			
		float distance = Distance(pos, mNodeMap[i]->mPosition);

		if (distance < minDistance)
		{
			minDistance = distance;
			index = i;
		}
	}

	return mNodeMap[index]->mPosition;
}

vector<Vector3> AStar::FindPath(int start, int end)
{
	ResetNodeState();

	float G = 0.0f;
	float H = getDistance(start, end);

	mNodeMap[start]->mG = G;
	mNodeMap[start]->mH = H;
	mNodeMap[start]->mF = G + H;
	mNodeMap[start]->mVia = start;
	mNodeMap[start]->mState = Node::OPEN;

	mHeap->Insert(mNodeMap[start]);

	while (mNodeMap[end]->mState != Node::CLOSED)
	{
		int curIndex = getMinNode(); // F값 가장 적은거.
		extend(curIndex, end);
		mNodeMap[curIndex]->mState = Node::CLOSED;
	}

	vector<Vector3> path;

	int curIndex = end;

	while (curIndex != start)
	{
		mNodeMap[curIndex]->mState = Node::USING;
		path.emplace_back(mNodeMap[curIndex]->mPosition);
		curIndex = mNodeMap[curIndex]->mVia;
	}

	mNodeMap[curIndex]->mState = Node::USING;
	path.emplace_back(mNodeMap[curIndex]->mPosition);

	mHeap->Clear();

	return path;
}


void AStar::MakeDirectPath(IN Vector3 start, IN Vector3 end, OUT vector<Vector3>& path)
{
	//캐릭터 위치값, 목적지, 경로벡터(
	int cutNodeNum = 0;
	Ray ray;
	ray.position = start;

	for (UINT i = 0; i < path.size(); i++) // 목적지 -> 시작지. 역순으로 들어있음.
	{
		ray.direction = path[i] - ray.position; // 캐릭터에서 path[i]까지의 방향벡터.
		float distance = ray.direction.Length(); // path[i]까지 거리.
		ray.direction.Normalize();

		// ray.direction : 캐릭터->path[i]까지 방향벡터.
		// ray.position = start
		// distance : 캐릭터->path[i]까지 거리..

		if (!CollisionObstacle(ray, distance)) // 캐릭터와 path[i]사이에 장애물이 없다면...!
		{
			cutNodeNum = path.size() - i - 1;  
			break;
		}
	}

	for (int i = 0; i < cutNodeNum; i++) // 캐릭터에서 다이렉트로 갈수있는 곳까지 전부 빼버리기.
	{
		path.pop_back();
	}

	cutNodeNum = 0;
	ray.position = end;

	for (UINT i = 0; i < path.size(); i++) 
	{
		ray.direction = path[path.size() - i - 1] - ray.position; 
		float distance = ray.direction.Length();

		ray.direction.Normalize();

		if (!CollisionObstacle(ray, distance)) // 충돌 안하면
		{
			cutNodeNum = path.size() - i - 1;
			break;
		}
	}

	for (int i = 0; i < cutNodeNum; i++)
	{
		path.erase(path.begin());
	}
}

bool AStar::CollisionObstacle(Ray ray, float destDistance)
{
	for (Collider* obstacle : mObstacles)
	{
		Contact contact;

		if (obstacle->RayCollision(ray, &contact)) // ray방향에 장애물이 있다면 (방향만 체크)
		{
			if (contact.distance < destDistance) // 목표지점 사이에 장애물이 있다면.
				return true;
		}
	}

	return false;
}

void AStar::ResetNodeState() // 장애물 제외 전부 NONE
{
	for (Node* node : mNodeMap)
	{
		if (node->mState != Node::OBSTACLE)
			node->mState = Node::NONE;
	}
}

void AStar::SetCheckFalse()
{
	for (int i = 0; i < mNodeMap.size(); i++)
	{
		mNodeMap[i]->SetTestNode(false);
	}
}

float AStar::getDistance(int curIndex, int end)
{
	Vector3 startPos = mNodeMap[curIndex]->mPosition;
	Vector3 endPos = mNodeMap[end]->mPosition;

	return Distance(startPos, endPos);
}

void AStar::extend(int center, int end)
{
	vector<Node::EdgeInfo*> edges = mNodeMap[center]->mEdges;

	for (UINT i = 0; i < edges.size(); i++)
	{
		int index = edges[i]->index;

		if (mNodeMap[index]->mState == Node::CLOSED ||   //OPEN 이여야 건너뜀.
			mNodeMap[index]->mState == Node::OBSTACLE)
			continue;

		float G = mNodeMap[center]->mG + edges[i]->edgeCost;
		float H = getDistance(index, end);
		float F = G + H;

		if (mNodeMap[index]->mState == Node::OPEN)
		{
			if (F < mNodeMap[index]->mF)
			{
				mNodeMap[index]->mG = G;
				mNodeMap[index]->mF = F;
				mNodeMap[index]->mVia = center;
			}
		}
		else if (mNodeMap[index]->mState == Node::NONE)
		{
			mNodeMap[index]->mG = G;
			mNodeMap[index]->mH = H;
			mNodeMap[index]->mF = F;
			mNodeMap[index]->mVia = center;
			mNodeMap[index]->mState = Node::OPEN;

			mHeap->Insert(mNodeMap[index]);
		}
	}
}

int AStar::getMinNode()
{
	return mHeap->DeleteRoot()->mIndex;
}

//void AStar::SetTestNode(int index)
//{
//	mNodes[index]->mbIsCheck = true;
//}