#include "Framework.h"

Monster::Monster() :
	ModelAnimator(1,MAX_BONE,MAX_FRAME_KEY),
	mDamage(100.0f),
	mMaxHP(100.0f),
	mCurHP(0.0f),
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
	mBeforeTargetPosition(0.0f, 0.0f, 0.0f),
	mCurrentTargetPosition(0.0f, 0.0f, 0.0f),
	mbPathSizeCheck(false),
	mInstanceIndex(0),
	mbIsCompletedAnim(false),
	mbIsStartedAnim(false)
{
	mCurHP = mMaxHP;
	mPathUpdatePeriodFuncPointer = bind(&Monster::SetRealtimeAStarPath, this, placeholders::_1);
	mRotationPeriodFuncPointer = bind(&Transform::RotateToDestinationForModel, this, placeholders::_1, placeholders::_2);

	mPathNodesCheck.assign(true, mPathNodesCheckSize);
	mStatusBar = new MonsterStatusBar();
}

Monster::~Monster()
{
	GM->SafeDelete(mAStar);
	GM->SafeDelete(mModelAnimator);
	GM->SafeDelete(mStatusBar);
}

void Monster::SetAStarPath(Vector3 destPos) // ��ǥ�������� ��μ���. mPath Update.
{
	mPath.clear();
	mAStar->ResetNodeState(); // ��ֹ� ����, ��� Node�� State�� None���� �ʱ�ȭ.

	if (mbHasTerrainObstacles) // �ͷ����� ��ֹ���������.
	{
		setObstaclesTerrain(destPos);
	}
	else
	{
		setNoneObstaclesTerrain(destPos);
	}
}

void Monster::SetPortraitTexture(Texture* texture)
{
	mStatusBar->SetPortraitTexture(texture);
}

void Monster::SetCurMainCamera(Camera* mainCamera)
{
	mCurMainCamera = mainCamera; 
	mStatusBar->SetCurMainCamera(mainCamera);
}

void Monster::setObstaclesTerrain(Vector3 destPos)
{
	Ray ray;
	ray.position = mPosition;
	ray.direction = (destPos - mPosition).Normal();
	float distance = Distance(destPos, mPosition);

	if (mAStar->CollisionObstacle(ray, distance)) // �� ��ġ�� ��ǥ�������̿� ��ֹ��� �ִٸ�
	{
		int startIndex = mAStar->FindCloseNode(mPosition); // ���� position������ ���� ����� ��� �ε���.
		int endIndex = mAStar->FindCloseNode(destPos); // ��ǥ position������ ���� ����� ��� �ε���.

		mPath = mAStar->FindPath(startIndex, endIndex); // ��λ���.
		mPath.insert(mPath.begin(), destPos); // ��ǥ��ġ�� ��κ��� �� �տ� �ֱ�. 

		mAStar->MakeDirectPath(mPosition, destPos, mPath); // mPath�� ĳ���Ͱ� ���̷�Ʈ�� �����ִ� ��� �Ѱ��� ���´�.
		mPath.insert(mPath.begin(), destPos); //�� ���������ϱ� �ٽ� �־��ֱ�

		UINT pathSize = mPath.size();

		while (mPath.size() > 2)
		{
			vector<Vector3> tempPath;

			for (UINT i = 1; i < mPath.size() - 1; i++) // ������,����� ������ ���.
			{
				tempPath.emplace_back(mPath[i]);
			}

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
		//mAStar->SetDirectNode(mAStar->FindCloseNode(destPos)); // ���̷�Ʈ�� �����ִ� ���� ����� ����.
		mPath.insert(mPath.begin(), destPos);
	}
}

void Monster::setNoneObstaclesTerrain(Vector3 destPos)
{
	mPath.insert(mPath.begin(), destPos);
}

void Monster::SetRealtimeAStarPath(Vector3 destPos) // AStar �ǽð���
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

		if (mAStar->CollisionObstacle(ray, distance)) // ��ο� ��ֹ��� �ִٸ� // ray�� ���������� �Ÿ�.
		{
			int startIndex = mAStar->FindCloseNode(mPosition); // ���� position������ ���� ����� ��� �ε���.
			int endIndex = mAStar->FindCloseNode(destPos); // ��ǥ position������ ���� ����� ��� �ε���.

			mPath = mAStar->FindPath(startIndex, endIndex); // ��λ���.
			mPath.insert(mPath.begin(), destPos); // ��ǥ��ġ�� ��κ��� �� �տ� �ֱ�. 

			mAStar->MakeDirectPath(mPosition, destPos, mPath); // path���Ϳ� ĳ���Ͱ� ���̷�Ʈ�� �����ִ� ��� �Ѱ��� ���´�.
			mPath.insert(mPath.begin(), destPos); //�� ���������ϱ� �ٽ� �ֱ�.

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
			//mPath.push_back(destPos);
		}


		// mPath ��� ���� �Ϸ�.

		for (int i = 0; i < mPath.size(); i++)
		{
			mPathNodesCheck[i] = true;
		}


		if (mPath.size() == 1 && !mbPathSizeCheck) // ���Ͱ� ĳ���Ϳ��� ���̷�Ʈ��(mPath.size() == 1) �޷��� �� �����ֱ⸶�� ȸ���ϴ°� �������� ���ǹ�...
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
		/*mAStar->SetCheckFalse();
		for (int i = 0; i < mPath.size(); i++)
		{
			int t = mAStar->FindCloseNode(mPath[i]);
			mAStar->SetTestNode(t);
		}*/
	}
} 

void Monster::MoveToDestUsingAStar(Vector3 dest) // �ǽð���
{
	ExecuteAStarUpdateFunction(mPathUpdatePeriodFuncPointer, dest, mAStarPathUpdatePeriodTime); // �ʴ� �ѹ��� ��ξ�����Ʈ. ���������� mPath.back()���� Ÿ���� ��ġ����. �ٷ� �� ���� Ÿ�ٰ� ���� ����� ���.
	//ExecuteRotationPeriodFunction(mRotationPeriodFuncPointer, this, mPath.back(), 1.0f);

	if (mPath.size() > 0)
	{
		mTargetNode = mPath.back(); // ���� �������.
		mTargetNodeDirVector3 = (mTargetNode - mPosition).Normal();

		if (CompareVector3XZ(mTargetNodeDirVector3, mBeforeDirVector3 * -1.0f)) // �ݵ�� ���� ���� �̵� ��, ����.
		{
			mIsAStarPathUpdate = false; // ���� ���ϰ� ���Ƴ���
			mPath.push_back(mBeforeNode); // ���� ���(5) �־���� ����� �̵�.
			mTargetNode = mBeforeNode;
			mTargetNodeDirVector3 = mBeforeDirVector3;

			mPathNodesCheck[mPath.size() - 1] = true;
		}

		MoveToDestination(this, mTargetNode, mMoveSpeed);
		
		if (mPathNodesCheck[mPath.size() - 1]) // true�� ȸ��.
		{
			RotateToDestinationForModel(this, mTargetNode);
			mPathNodesCheck[mPath.size() - 1] = false; // �ѹ��� Rotation ���Ѿ��ϱ� ������ ��ٴ�.
		}

		// mMonster->SetAStarPath�� �ð��� ����Ǵ��� mIsAStarPathUpdate = true�϶��� ����.

		float length = (mTargetNode - mPosition).Length();

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

void Monster::UIUpdate()
{
	Vector3 prevPosition = { mPosition.x, mPosition.y + 8.0f, mPosition.z };
	Vector3 screenPosition = WorldToScreen(prevPosition, mCurMainCamera);
	
	mStatusBar->mPosition = screenPosition + mUIOffset;
	mStatusBar->mPosition.z = this->mPosition.z;
	mStatusBar->Update();
}

void Monster::UIRender()
{
	mStatusBar->Render();
	/*mStatusBar->PostTransformRender();
	mStatusBar->PostRender();*/
}


