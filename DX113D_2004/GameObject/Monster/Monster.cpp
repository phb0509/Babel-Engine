#include "Framework.h"

Monster::Monster() :
	mMoveSpeed(10.0f),
	mDamage(100.0f),
	mMaxHP(1000.0f),
	mCurrentHP(0.0f),
	mAStar(new AStar()),
	mTerrain(nullptr),
	mbIsStalk(false),
	mDistanceToPlayer(0.0f),
	mPlayerDetectRange(30.0f),
	mDistanceToPlayerForAttack(20.0f),
	mAStarPathUpdatePeriodTime(1.0f),
	mPlayer(nullptr),
	mCurDirVector3(0.0f, 0.0f, 0.0f),
	mCurNode(0.0f, 0.0f, 0.0f),
	mBeforeDirVector3(0.0f, 0.0f, 0.0f),
	mBeforeNode(0.0f, 0.0f, 0.0f)
{
	mCurrentHP = mMaxHP;
	mPatrolState = new PatrolState();
	mStalkingState = new StalkingState();
	mAttackState = new AttackState();

	mPeriodFuncPointer = bind(&Monster::SetRealtimeAStarPath, this, placeholders::_1);
}

Monster::~Monster()
{
	delete mPatrolState;
	delete mStalkingState;
	delete mAttackState;
}



void Monster::SetAStarPath(Vector3 destPos)
{
	mPath.clear();
	mAStar->Reset();

	Ray ray;
	ray.position = position;
	ray.direction = (destPos - position).Normal();
	float distance = Distance(destPos, position);

	if (mAStar->CollisionObstacle(ray, distance)) // 현 위치와 목표지점사이에 장애물이 있다면
	{
		int startIndex = mAStar->FindCloseNode(position); // 현재 position값에서 가장 가까운 노드 인덱스.
		int endIndex = mAStar->FindCloseNode(destPos); // 목표 position값에서 가장 가까운 노드 인덱스.

		mPath = mAStar->FindPath(startIndex, endIndex); // 경로생성.
		mPath.insert(mPath.begin(), destPos); // 목표위치를 경로벡터 맨 앞에 넣기. 

		mAStar->MakeDirectPath(position, destPos, mPath); // path벡터에 캐릭터가 다이렉트로 갈수있는 노드 한개만 남는다.
		mPath.insert(mPath.begin(), destPos); //다 삭제됐으니까 다시 넣어주는구나

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

	else // 그냥 출발지에서 바로 다이렉트로 갈수있으면.
	{
		mAStar->SetDirectNode(mAStar->FindCloseNode(destPos)); // 다이렉트로 갈수있는 노드는 노란색 설정.
		mPath.insert(mPath.begin(), destPos);
	}

}




void Monster::SetRealtimeAStarPath(Vector3 destPos) // 실시간용.
{
	if (mIsAStarPathUpdate)
	{
		mPath.clear();
		mAStar->Reset();

		Ray ray;
		ray.position = position;
		ray.direction = (destPos - position).Normal();
		float distance = Distance(destPos, position);

		if (mAStar->CollisionObstacle(ray, distance)) // 경로에 장애물이 있다면 // ray와 목적지까지 거리.
		{
			int startIndex = mAStar->FindCloseNode(position); // 현재 position값에서 가장 가까운 노드 인덱스.
			int endIndex = mAStar->FindCloseNode(destPos); // 목표 position값에서 가장 가까운 노드 인덱스.

			mPath = mAStar->FindPath(startIndex, endIndex); // 경로생성.
			mPath.insert(mPath.begin(), destPos); // 목표위치를 경로벡터 맨 앞에 넣기. 

			mAStar->MakeDirectPath(position, destPos, mPath); // path벡터에 캐릭터가 다이렉트로 갈수있는 노드 한개만 남는다.
			mPath.insert(mPath.begin(), destPos); //다 삭제됐으니까 다시 넣어주는구나

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
			mAStar->SetDirectNode(mAStar->FindCloseNode(destPos)); // 다이렉트로 갈수있는 노드는 노란색 설정.
			mPath.insert(mPath.begin(), destPos);
		}
	}
}

void Monster::MoveToDestUsingAStar(Vector3 dest) // 실시간용
{
	ExecuteAStarUpdateFunction(mPeriodFuncPointer, dest, mAStarPathUpdatePeriodTime); // 초당 한번씩 경로업데이트.
	
	if (mPath.size() > 0)
	{
		mCurNode = mPath.back();
		mCurDirVector3 = (mCurNode - position).Normal();

		MoveToDestination(this, mCurNode, mMoveSpeed);

		if (CompareVector3XZ(mCurDirVector3, mBeforeDirVector3)) // 반드시 다음 노드로 이동 후, 갱신.
		{
			mIsAStarPathUpdate = false; // 갱신 못하게 막아놓고
			mPath.push_back(mBeforeNode); // 이전 노드(5) 넣어놓고 여기로 이동.
			mCurDirVector3 = mBeforeDirVector3;
			mCurNode = mBeforeNode;
		}

		// monster->SetAStarPath는 시간이 경과되더라도 mIsAStarPathUpdate = true일때만 갱신.

		float length = (mCurNode - position).Length();

		if (length < 1.0f) // 노드에 도착하면
		{
			mPath.pop_back();
			mIsAStarPathUpdate = true;
		}

		mBeforeDirVector3 = mCurDirVector3;
		mBeforeNode = mCurNode;
	}
}


float Monster::GetDistanceToPlayer()
{
	mPlayer = GM->Get()->GetPlayer();
	mDistanceVector3ToPlayer = mPlayer->position - position;
	mDistanceToPlayer = sqrt(pow(mDistanceVector3ToPlayer.x, 2) + pow(mDistanceVector3ToPlayer.z, 2));

	return mDistanceToPlayer;
}

void Monster::SetTerrain(Terrain* value)
{
	mTerrain = value;
	mAStar->SetNode(mTerrain);

}

void Monster::ChangeState(State* nextState)
{
	//static_assert(mCurrentState && nextState);

	mCurrentState->Exit(this);

	mCurrentState = nextState;

	mCurrentState->Enter(this);
}





