#include "Framework.h"
#include "PatrolState.h"

PatrolState::PatrolState() :
	mPatrolRandomNum(0.0f),
	mbPatrolMove(false),
	mbSetPatrolDest(true),
	mCurrentTime(0.0f),
	mStandTime(0.0f),
	mPatrolDestPos(0.0f, 0.0f, 0.0f)
{
}

PatrolState::~PatrolState()
{
}

void PatrolState::Initialize()
{
	mPatrolRandomNum = 0.0f;
	mbPatrolMove = false;
	mbSetPatrolDest = true;
	mCurrentTime = 0.0f;
	mStandTime = 0.0f;
	mPatrolDestPos = { 0.0f,0.0f,0.0f };
}

void PatrolState::SetPatrolTargetPoint(Vector3& patrolTargetPoint)
{
	
}

void PatrolState::Enter(Monster* mMonster)
{
	Initialize();
}

void PatrolState::Execute(Monster* mMonster)
{
	mPlayer = GM->Get()->GetPlayer();

	if (!mbPatrolMove) // ���� ���� ���� üũ. 
	{
		if (Timer::Get()->GetRunTime() >= mStandTime)
		{
			mbSetPatrolDest = true;
		}
	}

	if (mbSetPatrolDest && !mbPatrolMove) // IDLE���� ���� �ð� ������, �������. ��ǥ���� �����ϰ� ���ʹ������� ȸ��. PatrolState���� �� ó�� ����.
	{
		// ������ǥ��ǥ ����.
		float patrolDestPosCorrectionValue = 50.0f;
		mPatrolDestPos.x = Random(mMonster->mPosition.x - patrolDestPosCorrectionValue, mMonster->mPosition.x + patrolDestPosCorrectionValue);
		mPatrolDestPos.z = Random(mMonster->mPosition.z - patrolDestPosCorrectionValue, mMonster->mPosition.z + patrolDestPosCorrectionValue);

		// ������ǥ������ ��ũ�⸦ ��� ��� ����ó��.
		float patrolRangeCorrectionValue = 30.0f;

		if (mPatrolDestPos.x >= mMonster->GetTerrain()->GetSize().x)
		{
			mPatrolDestPos.x = mMonster->GetTerrain()->GetSize().x - patrolRangeCorrectionValue;
		}
		else if (mPatrolDestPos.x <= 0.0f)
		{
			mPatrolDestPos.x = patrolRangeCorrectionValue;
		}

		if (mPatrolDestPos.z >= mMonster->GetTerrain()->GetSize().y)
		{
			mPatrolDestPos.z = mMonster->GetTerrain()->GetSize().y - patrolRangeCorrectionValue;
		}
		else if (mPatrolDestPos.z <= 0.0f)
		{
			mPatrolDestPos.z = patrolRangeCorrectionValue;
		}

		mbSetPatrolDest = false;
		mbPatrolMove = true;

		mPatrolDestPos = mMonster->GetAStar()->FindCloseNodePosition(mPatrolDestPos);

		mMonster->GetPath().clear();
		mMonster->SetAStarPath(mPatrolDestPos);
		mMonster->RotateToDestinationForModel(mMonster, mMonster->GetPath().back());
	}

	else if (!mbSetPatrolDest && mbPatrolMove) // �����̵�.
	{		
		if (Timer::Get()->GetFPS() > 10) // �ʱ����� FPS���� ���������� 1�� �Ǵ°Ϳ� ���� ���׷� ���� ����ó��. 
		{
			//mMonster->MoveToDestUsingAStar(mPatrolDestPos);

			float length = (mMonster->GetPath().back() - mMonster->mPosition).Length();

			if (length < 1.0f) // ��忡 �����ϸ�
			{
				mMonster->GetPath().pop_back();

				if (mMonster->GetPath().size() > 0)
				{
					mMonster->RotateToDestinationForModel(mMonster, mMonster->GetPath().back());
				}
			}

			if (mMonster->GetPath().size() > 0)
			{
				mMonster->MoveToDestination(mMonster, mMonster->GetPath().back(), mMonster->GetMoveSpeed());
			}
			
			mMonster->SetAnimation(eAnimationStates::Run); // Run.
		}

		if (fabs(mPatrolDestPos.x - mMonster->mPosition.x) < 1.0f && // ��ǥ���� �����ϸ�
			fabs(mPatrolDestPos.z - mMonster->mPosition.z) < 1.0f)
		{
			mbPatrolMove = false;
			mPatrolRandomNum = GameMath::Random(5.0f, 10.0f); // ���� ����.
			mCurrentTime = Timer::Get()->GetRunTime();
			mStandTime = mCurrentTime + mPatrolRandomNum;

			mMonster->SetAnimation(eAnimationStates::Idle);
		}
	}

	//���� �� �÷��̾� �ִ��� üũ.
	if (mMonster->GetDistanceToPlayer() <= mMonster->GetPlayerDetectRange()) // �÷��̾ �Ÿ� �ȿ� ������.
	{
		mMonster->SetIsStalk(true);
		mMonster->ChangeState(mMonster->GetStalkingState()); // Stalking���� ������ȯ.
	}
}

void PatrolState::Exit(Monster* mMonster)
{
}
