#include "Framework.h"
#include "PatrolState.h"

PatrolState::PatrolState() :
	mPatrolRandomNum(0.0f),
	mbIsPatrolMove(false),
	mbIsSetPatrolDest(true),
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
	mbIsPatrolMove = false;
	mbIsSetPatrolDest = true;
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

void PatrolState::Execute(Monster* monster)
{
	mPlayer = GM->Get()->GetPlayer();

	if (!mbIsPatrolMove) // ���� ���� ���� üũ. 
	{
		if (Timer::Get()->GetRunTime() >= mStandTime)
		{
			mbIsSetPatrolDest = true; // ������ǥ��ǥ������ �����ؾ��Ѵٴ� ǥ��.
		}
	}

	if (mbIsSetPatrolDest && !mbIsPatrolMove) // IDLE���� ���� �ð� ������, �������. ��ǥ���� �����ϰ� ���ʹ������� ȸ��. PatrolState���� �� ó�� ����.
	{
		// ������ǥ��ǥ ����.
		float patrolDestPosCorrectionValue = 50.0f; // ����������
		mPatrolDestPos.x = Random(monster->mPosition.x - patrolDestPosCorrectionValue, monster->mPosition.x + patrolDestPosCorrectionValue);
		mPatrolDestPos.z = Random(monster->mPosition.z - patrolDestPosCorrectionValue, monster->mPosition.z + patrolDestPosCorrectionValue);

		mbIsSetPatrolDest = false; // ������ǥ��ǥ���� �������� false.
		mbIsPatrolMove = true; // ���� �̵��ؾ��ϴ� true.

		// ������ǥ������ ��ũ�⸦ ��� ��� ����ó��.
		float patrolRangeCorrectionValue = 30.0f;

		if (mPatrolDestPos.x >= monster->GetTerrain()->GetSize().x)
		{
			mPatrolDestPos.x = monster->GetTerrain()->GetSize().x - patrolRangeCorrectionValue;
		}
		else if (mPatrolDestPos.x <= 0.0f)
		{
			mPatrolDestPos.x = patrolRangeCorrectionValue;
		}

		if (mPatrolDestPos.z >= monster->GetTerrain()->GetSize().y)
		{
			mPatrolDestPos.z = monster->GetTerrain()->GetSize().y - patrolRangeCorrectionValue;
		}
		else if (mPatrolDestPos.z <= 0.0f)
		{
			mPatrolDestPos.z = patrolRangeCorrectionValue;
		}

		//mbIsSetPatrolDest = false;
		//mbIsPatrolMove = true;

		mPatrolDestPos = monster->GetAStar()->FindCloseNodePosition(mPatrolDestPos); // ��ǥ��ǥ���� ���� ����� �����ġ ����.�׷��� ����??

		//monster->GetPath().clear(); // SetAStarPath���� ��ó���� ��������. 
		monster->SetAStarPath(mPatrolDestPos);
		monster->RotateToDestinationForModel(monster, monster->GetPath().back());
	}

	else if (!mbIsSetPatrolDest && mbIsPatrolMove) // �����̵�.
	{		
		if (Timer::Get()->GetFPS() > 10) // �ʱ����� FPS���� ���������� 1�� �Ǵ°Ϳ� ���� ���׷� ���� ����ó��. 
		{
			//mMonster->MoveToDestUsingAStar(mPatrolDestPos);

			float length = (monster->GetPath().back() - monster->mPosition).Length();

			if (length < 1.0f) // ��忡 �����ϸ�
			{
				monster->GetPath().pop_back();

				if (monster->GetPath().size() > 0)
				{
					monster->RotateToDestinationForModel(monster, monster->GetPath().back());
				}
			}

			if (monster->GetPath().size() > 0)
			{
				monster->MoveToDestination(monster, monster->GetPath().back(), monster->GetMoveSpeed());
			}
			
			monster->SetAnimation(eAnimationStates::Run); // Run.
		}

		if (fabs(mPatrolDestPos.x - monster->mPosition.x) < 1.0f && // ��ǥ���� �����ϸ�
			fabs(mPatrolDestPos.z - monster->mPosition.z) < 1.0f)
		{
			mbIsPatrolMove = false;
			mPatrolRandomNum = GameMath::Random(5.0f, 10.0f); // ���� ����.
			mCurrentTime = Timer::Get()->GetRunTime();
			mStandTime = mCurrentTime + mPatrolRandomNum;

			monster->SetAnimation(eAnimationStates::Idle);
		}
	}

	//���� �� �÷��̾� �ִ��� üũ.
	if (monster->GetDistanceToPlayer() <= monster->GetPlayerDetectRange()) // �÷��̾ �Ÿ� �ȿ� ������.
	{
		monster->SetIsStalk(true);
		monster->ChangeState(monster->GetStalkingState()); // Stalking���� ������ȯ.
	}
}

void PatrolState::Exit(Monster* mMonster)
{
}
