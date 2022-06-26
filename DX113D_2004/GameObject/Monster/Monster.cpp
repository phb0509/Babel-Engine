#include "Framework.h"

Monster::Monster() :
	ModelAnimator(),
	mDamage(100.0f),
	mMaxHP(100.0f),
	mCurrentHP(0.0f),
	mAStar(new AStar()),
	mTerrain(nullptr),
	mDistanceToPlayer(0.0f),
	mPlayerDetectRange(30.0f),
	mDistanceToPlayerForAttack(20.0f),
	mAStarPathUpdatePeriodTime(1.0f),
	mPlayer(nullptr),
	mTargetNodeDirVector3(0.0f, 0.0f, 0.0f),
	mTargetNode(0.0f, 0.0f, 0.0f),
	mBeforeDirVector3(0.0f, 0.0f, 0.0f),
	mBeforeNode(0.0f, 0.0f, 0.0f),
	mPathNodesCheckSize(200),
	mBeforeTargetPosition(0.0f,0.0f,0.0f),
    mCurrentTargetPosition(0.0f,0.0f,0.0f),
	mbPathSizeCheck(false),
	mInstanceIndex(0)
{
	mCurrentHP = mMaxHP;
	mPathUpdatePeriodFuncPointer = bind(&Monster::SetRealtimeAStarPath, this, placeholders::_1);
	mRotationPeriodFuncPointer = bind(&Transform::RotateToDestinationForModel, this, placeholders::_1, placeholders::_2);

	mPathNodesCheck.assign(true, mPathNodesCheckSize);
}

Monster::~Monster()
{
	delete mAStar;
	delete mModelAnimator;
}

void Monster::SetAStarPath(Vector3 destPos) // 목표지점으로 경로설정. mPath Update.
{
	mPath.clear();
	mAStar->ResetNodeState(); // 장애물 제외, 모든 Node의 State를 None으로 초기화.

	if (mbHasTerrainObstacles) // 터레인의 장애물보유여부.
	{
		setObstaclesTerrain(destPos);
	}
	else
	{
		setNoneObstaclesTerrain(destPos);
	}
}

void Monster::setObstaclesTerrain(Vector3 destPos)
{
	Ray ray;
	ray.position = mPosition;
	ray.direction = (destPos - mPosition).Normal();
	float distance = Distance(destPos, mPosition);

	if (mAStar->CollisionObstacle(ray, distance)) // 현 위치와 목표지점사이에 장애물이 있다면
	{
		int startIndex = mAStar->FindCloseNode(mPosition); // 현재 position값에서 가장 가까운 노드 인덱스.
		int endIndex = mAStar->FindCloseNode(destPos); // 목표 position값에서 가장 가까운 노드 인덱스.

		mPath = mAStar->FindPath(startIndex, endIndex); // 경로생성.
		mPath.insert(mPath.begin(), destPos); // 목표위치를 경로벡터 맨 앞에 넣기. 

		mAStar->MakeDirectPath(mPosition, destPos, mPath); // mPath에 캐릭터가 다이렉트로 갈수있는 노드 한개만 남는다.
		mPath.insert(mPath.begin(), destPos); //다 삭제됐으니까 다시 넣어주기

		UINT pathSize = mPath.size();

		while (mPath.size() > 2)
		{
			vector<Vector3> tempPath;

			for (UINT i = 1; i < mPath.size() - 1; i++) // 목적지,출발지 사이의 경로.
			{
				tempPath.emplace_back(mPath[i]);
			}

			Vector3 start = mPath.back(); // 출발지
			Vector3 end = mPath.front(); // 목적지.

			mAStar->MakeDirectPath(start, end, tempPath);

			mPath.clear();
			mPath.emplace_back(end);

			for (Vector3 temp : tempPath)
				mPath.emplace_back(temp);

			mPath.emplace_back(start);

			if (pathSize == mPath.size())
				break;
			else
				pathSize = mPath.size();
		}
	}

	else // 그냥 출발지에서 바로 다이렉트로 갈수있으면.
	{
		//mAStar->SetDirectNode(mAStar->FindCloseNode(destPos)); // 다이렉트로 갈수있는 노드는 노란색 설정.
		mPath.insert(mPath.begin(), destPos);
	}
}

void Monster::setNoneObstaclesTerrain(Vector3 destPos)
{
	mPath.insert(mPath.begin(), destPos);
}

void Monster::SetRealtimeAStarPath(Vector3 destPos) // AStar 실시간용
{
	if (mIsAStarPathUpdate)
	{
		mCurrentTargetPosition = destPos;

		mPath.clear();
		mAStar->ResetNodeState();
		mPathNodesCheck.assign(mPathNodesCheckSize, false);

		Ray ray;
		ray.position = mPosition;
		ray.direction = (destPos - mPosition).Normal();
		float distance = Distance(destPos, mPosition);

		if (mAStar->CollisionObstacle(ray, distance)) // 경로에 장애물이 있다면 // ray와 목적지까지 거리.
		{
			int startIndex = mAStar->FindCloseNode(mPosition); // 현재 position값에서 가장 가까운 노드 인덱스.
			int endIndex = mAStar->FindCloseNode(destPos); // 목표 position값에서 가장 가까운 노드 인덱스.

			mPath = mAStar->FindPath(startIndex, endIndex); // 경로생성.
			mPath.insert(mPath.begin(), destPos); // 목표위치를 경로벡터 맨 앞에 넣기. 

			mAStar->MakeDirectPath(mPosition, destPos, mPath); // path벡터에 캐릭터가 다이렉트로 갈수있는 노드 한개만 남는다.
			mPath.insert(mPath.begin(), destPos); //다 삭제됐으니까 다시 넣기.

			UINT pathSize = mPath.size();

			while (mPath.size() > 2)
			{
				vector<Vector3> tempPath;

				for (UINT i = 1; i < mPath.size() - 1; i++) // 목적지,출발지 사이의 경로.
					tempPath.emplace_back(mPath[i]);

				Vector3 start = mPath.back(); // 출발지
				Vector3 end = mPath.front(); // 목적지.

				mAStar->MakeDirectPath(start, end, tempPath);

				mPath.clear();
				mPath.emplace_back(end);

				for (Vector3 temp : tempPath)
					mPath.emplace_back(temp);

				mPath.emplace_back(start);

				if (pathSize == mPath.size())
					break;
				else
					pathSize = mPath.size();
			}
		}
		else
		{
			//mAStar->SetDirectNode(mAStar->FindCloseNode(destPos)); // 다이렉트로 갈수있는 노드는 노란색 설정.
			mPath.insert(mPath.begin(), destPos);
			//mPath.push_back(destPos);
		}


		// mPath 요소 삽입 완료.

		for (int i = 0; i < mPath.size(); i++)
		{
			mPathNodesCheck[i] = true;
		}


		if (mPath.size() == 1 && !mbPathSizeCheck) // 몬스터가 캐릭터에게 다이렉트로(mPath.size() == 1) 달려올 시 일정주기마다 회전하는걸 막기위한 조건문...
		{
			mbPathSizeCheck = true;
		}
		else if (mPath.size() == 1 && mbPathSizeCheck) // 경로갱신후,이전에도 사이즈가 1이였는데 이번에도 1일경우
		{
			if ((mBeforeTargetPosition - mCurrentTargetPosition).Length() < 1.0f) // 타겟 위치가 이전과 거의 차이가 없으면(제자리라면)
			{
				mPathNodesCheck[0] = false;
			}
		}

		mBeforeTargetPosition = mCurrentTargetPosition;

		// mPath 내부 노드들 인게임내에서 확인하기위한 코드(보라색으로). 없어도 상관없다.
		/*mAStar->SetCheckFalse();
		for (int i = 0; i < mPath.size(); i++)
		{
			int t = mAStar->FindCloseNode(mPath[i]);
			mAStar->SetTestNode(t);
		}*/
	}
} 

void Monster::MoveToDestUsingAStar(Vector3 dest) // 실시간용
{
	ExecuteAStarUpdateFunction(mPathUpdatePeriodFuncPointer, dest, mAStarPathUpdatePeriodTime); // 초당 한번씩 경로업데이트. 최종적으로 mPath.back()에는 타겟의 위치벡터. 바로 그 전은 타겟과 가장 가까운 노드.
	//ExecuteRotationPeriodFunction(mRotationPeriodFuncPointer, this, mPath.back(), 1.0f);

	if (mPath.size() > 0)
	{
		mTargetNode = mPath.back(); // 가장 가까운노드.
		mTargetNodeDirVector3 = (mTargetNode - mPosition).Normal();

		if (CompareVector3XZ(mTargetNodeDirVector3, mBeforeDirVector3 * -1.0f)) // 반드시 다음 노드로 이동 후, 갱신.
		{
			mIsAStarPathUpdate = false; // 갱신 못하게 막아놓고
			mPath.push_back(mBeforeNode); // 이전 노드(5) 넣어놓고 여기로 이동.
			mTargetNode = mBeforeNode;
			mTargetNodeDirVector3 = mBeforeDirVector3;

			
			mPathNodesCheck[mPath.size() - 1] = true;

			char buff[100];
			sprintf_s(buff, "mIsAStarPathUpdate : %d  Count : %d\n", mIsAStarPathUpdate, count++);
			OutputDebugStringA(buff);
		}

		MoveToDestination(this, mTargetNode, mMoveSpeed);
		
		if (mPathNodesCheck[mPath.size() - 1]) // true면 회전.
		{
			RotateToDestinationForModel(this, mTargetNode);
			mPathNodesCheck[mPath.size() - 1] = false; // 한번만 Rotation 시켜야하기 때문에 잠근다.
		}

		// mMonster->SetAStarPath는 시간이 경과되더라도 mIsAStarPathUpdate = true일때만 갱신.

		float length = (mTargetNode - mPosition).Length();

		if (length < 1.0f) // 노드에 도착하면... 이지만 맨 마지막에는 타겟위치벡터 자체
		{
			mPath.pop_back();
			mIsAStarPathUpdate = true; // 도착했으니 풀어주기.
		}

		mBeforeDirVector3 = mTargetNodeDirVector3;
		mBeforeNode = mTargetNode;
	}
}

float Monster::GetDistanceToPlayer()
{
	mPlayer = GM->Get()->GetPlayer();
	mDistanceVector3ToPlayer = mPlayer->mPosition - mPosition;
	mDistanceToPlayer = sqrt(pow(mDistanceVector3ToPlayer.x, 2) + pow(mDistanceVector3ToPlayer.z, 2));

	return mDistanceToPlayer;
}

void Monster::SetTerrain(Terrain* terrain, bool hasTerrainObstacles) 
{
	mTerrain = terrain;
	mbHasTerrainObstacles = hasTerrainObstacles;
	mAStar->SetNodeMap(terrain->GetNodeMap());
}

void Monster::ChangeState(MonsterState* nextState)
{
	mCurrentFSMState->Exit(this);
	mCurrentFSMState = nextState;
	mCurrentFSMState->Enter(this);
}





