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
	mIsAStarPathUpdate = false;

	Ray ray;
	ray.position = position;
	ray.direction = (destPos - position).Normal();
	float distance = Distance(destPos, position);

	if (mAStar->CollisionObstacle(ray, distance)) // ��ο� ��ֹ��� �ִٸ� // ray�� ���������� �Ÿ�.
	{
		int startIndex = mAStar->FindCloseNode(position); // ���� position������ ���� ����� ��� �ε���.
		int endIndex = mAStar->FindCloseNode(destPos); // ��ǥ position������ ���� ����� ��� �ε���.

		mPath = mAStar->FindPath(startIndex, endIndex); // ��λ���.
		mPath.insert(mPath.begin(), destPos); // ��ǥ��ġ�� ��κ��� �� �տ� �ֱ�. 

		mAStar->MakeDirectPath(position, destPos, mPath); // path���Ϳ� ĳ���Ͱ� ���̷�Ʈ�� �����ִ� ��� �Ѱ��� ���´�.
		mPath.insert(mPath.begin(), destPos); //�� ���������ϱ� �ٽ� �־��ִ±���

		UINT pathSize = mPath.size();

		while (mPath.size() > 2)
		{
			vector<Vector3> tempPath;

			for (UINT i = 1; i < mPath.size() - 1; i++) // ������,����� ������ ���.
				tempPath.emplace_back(mPath[i]);

			Vector3 start = mPath.back(); // �����
			Vector3 end = mPath.front(); // ������.

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
		mAStar->SetDirectNode(mAStar->FindCloseNode(destPos)); // ���̷�Ʈ�� �����ִ� ���� ����� ����.
		mPath.insert(mPath.begin(), destPos);
	}
}

void Monster::MoveToDestUsingAStar(Vector3 dest) 
{
	//bool t = CheckTime(1.0f);

	//ExecuteAStarUpdateFunction(mPeriodFuncPointer, dest, mAStarPathUpdatePeriodTime); // �ʴ� �ѹ��� ��ξ�����Ʈ.
	ExecuteAStarUpdateFunction(mPeriodFuncPointer, dest, 1.0f, mIsAStarPathUpdate); // �ʴ� �ѹ��� ��ξ�����Ʈ.

	if (mPath.size() > 0)
	{
		mIsAStarPathUpdate = false;

		MoveToDestination(this, mPath.back(), mMoveSpeed);

		float length = (mPath.back() - position).Length();

		if (length < 1.0f) // ��忡 �����ϸ�
		{
			mPath.pop_back();
			mIsAStarPathUpdate = true;
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





