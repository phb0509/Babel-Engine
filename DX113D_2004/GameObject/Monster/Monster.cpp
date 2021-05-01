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
	mTargetNodeDirVector3(0.0f, 0.0f, 0.0f),
	mTargetNode(0.0f, 0.0f, 0.0f),
	mBeforeDirVector3(0.0f, 0.0f, 0.0f),
	mBeforeNode(0.0f, 0.0f, 0.0f),
	mPathNodesCheckSize(200),
	mBeforeTargetPosition(0.0f,0.0f,0.0f),
    mCurrentTargetPosition(0.0f,0.0f,0.0f),
	mbPathSizeCheck(false)

{
	mCurrentHP = mMaxHP;
	mPatrolState = new PatrolState();
	mStalkingState = new StalkingState();
	mAttackState = new AttackState();

	mPathUpdatePeriodFuncPointer = bind(&Monster::SetRealtimeAStarPath, this, placeholders::_1);
	mRotationPeriodFuncPointer = bind(&Transform::RotateToDestination, this, placeholders::_1, placeholders::_2);

	
	mPathNodesCheck.assign(true, mPathNodesCheckSize);
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

	if (mAStar->CollisionObstacle(ray, distance)) // �� ��ġ�� ��ǥ�������̿� ��ֹ��� �ִٸ�
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

	else // �׳� ��������� �ٷ� ���̷�Ʈ�� ����������.
	{
	//	mAStar->SetDirectNode(mAStar->FindCloseNode(destPos)); // ���̷�Ʈ�� �����ִ� ���� ����� ����.
		mPath.insert(mPath.begin(), destPos);
	}

}




void Monster::SetRealtimeAStarPath(Vector3 destPos) // �ǽð���.
{
	if (mIsAStarPathUpdate)
	{
		mCurrentTargetPosition = destPos;

		mPath.clear();
		mAStar->Reset();
		mPathNodesCheck.assign(mPathNodesCheckSize, false);

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
			//mAStar->SetDirectNode(mAStar->FindCloseNode(destPos)); // ���̷�Ʈ�� �����ִ� ���� ����� ����.
			mPath.insert(mPath.begin(), destPos);
		}


		// mPath ��� ���� �Ϸ�.

		for (int i = 0; i < mPath.size(); i++)
		{
			mPathNodesCheck[i] = true;
		}


		if (mPath.size() == 1 && !mbPathSizeCheck)
		{
			mbPathSizeCheck = true;
		}
		else if (mPath.size() == 1 && mbPathSizeCheck) // ��ΰ�����,�������� ����� 1�̿��µ� �̹����� 1�ϰ��
		{
			if ((mBeforeTargetPosition - mCurrentTargetPosition).Length() < 1.0f) // Ÿ�� ��ġ�� ������ ���� ���̰� ������(���ڸ����)
			{
				mPathNodesCheck[0] = false;
			}
		}

		mBeforeTargetPosition = mCurrentTargetPosition;

		// mPath ���� ���� �ΰ��ӳ����� Ȯ���ϱ����� �ڵ�(���������). ��� �������.
		mAStar->SetCheckFalse();
		for (int i = 0; i < mPath.size(); i++)
		{
			int t = mAStar->FindCloseNode(mPath[i]);
			mAStar->SetTestNode(t);
		}
	}
}

void Monster::MoveToDestUsingAStar(Vector3 dest) // �ǽð���
{
	ExecuteAStarUpdateFunction(mPathUpdatePeriodFuncPointer, dest, mAStarPathUpdatePeriodTime); // �ʴ� �ѹ��� ��ξ�����Ʈ. ���������� mPath.back()���� Ÿ���� ��ġ����. �ٷ� �� ���� Ÿ�ٰ� ���� ����� ���.
	//ExecuteRotationPeriodFunction(mRotationPeriodFuncPointer, this, mPath.back(), 1.0f);

	if (mPath.size() > 0)
	{
		mTargetNode = mPath.back(); // ���� �������.
		mTargetNodeDirVector3 = (mTargetNode - position).Normal();

		if (CompareVector3XZ(mTargetNodeDirVector3, mBeforeDirVector3 * -1.0f)) // �ݵ�� ���� ���� �̵� ��, ����.
		{
			mIsAStarPathUpdate = false; // ���� ���ϰ� ���Ƴ���
			mPath.push_back(mBeforeNode); // ���� ���(5) �־���� ����� �̵�.
			mTargetNode = mBeforeNode;
			mTargetNodeDirVector3 = mBeforeDirVector3;
		}

		MoveToDestination(this, mTargetNode, mMoveSpeed);
		
		if (mPathNodesCheck[mPath.size() - 1]) // true�� ȸ��.
		{
			RotateToDestination(this, mTargetNode);
			mPathNodesCheck[mPath.size() - 1] = false; // �ѹ��� Rotation ���Ѿ��ϱ� ������ ��ٴ�.
		}

		// monster->SetAStarPath�� �ð��� ����Ǵ��� mIsAStarPathUpdate = true�϶��� ����.

		float length = (mTargetNode - position).Length();

		if (length < 1.0f) // ��忡 �����ϸ�... ������ �� ���������� Ÿ����ġ���� ��ü
		{
			mPath.pop_back();
			mIsAStarPathUpdate = true; // ���������� Ǯ���ֱ�.
		}

		mBeforeDirVector3 = mTargetNodeDirVector3;
		mBeforeNode = mTargetNode;
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





