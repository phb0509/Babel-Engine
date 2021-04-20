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
	mPlayer(nullptr)
{
	mCurrentHP = mMaxHP;
	mPatrolState = new PatrolState();
	mStalkingState = new StalkingState();
	mAttackState = new AttackState();

	mPeriodFuncPointer = bind(&Monster::SetAStarPath, this,placeholders::_1);
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
		//GetAStar()->FindCloseNode(destPos)
		mAStar->SetDirectNode(mAStar->FindCloseNode(destPos)); // 다이렉트로 갈수있는 노드는 노란색 설정.
		mPath.insert(mPath.begin(), destPos);
	}
}

void Monster::MoveToDestUsingAStar(Vector3 dest) 
{
	//ExecuteAStarUpdateFunction(mPeriodFuncPointer, dest, mAStarPathUpdatePeriodTime); // 초당 한번씩 경로업데이트.
	ExecuteAStarUpdateFunction(mPeriodFuncPointer, dest, 1.0f); // 초당 한번씩 경로업데이트.

	if (mPath.size() > 0)
	{
		MoveToDestination(this, mPath.back(), mMoveSpeed);

		char buff[200];
		sprintf_s(buff, "mPath.back.x : %f , y : %f , Size : %d\n", mPath.back().x, mPath.back().z, mPath.size());
		OutputDebugStringA(buff);



		float length = (mPath.back() - position).Length();

		if (length < 1.0f)
		{
			mPath.pop_back();
		}
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





