#include "Framework.h"

Patrol::Patrol() :
	mPatrolRandomNum(0.0f),
	mbPatrolMove(false),
	mbSetPatrolDest(true),
	mCurrentTime(0.0f),
	mStandTime(0.0f),
	mPatrolDestPos(0.0f, 0.0f, 0.0f),
	mDistanceToPlayer(0.0f)
{
}

Patrol::~Patrol()
{
}

void Patrol::Initialize()
{
	mPatrolRandomNum = 0.0f;
	mbPatrolMove = false;
	mbSetPatrolDest = true;
	mCurrentTime = 0.0f;
	mStandTime = 0.0f;
	mPatrolDestPos = { 0.0f,0.0f,0.0f };
}



void Patrol::Enter(Mutant* mutant)
{
	Initialize();
}

void Patrol::Execute(Mutant* mutant)
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
		mPatrolDestPos.x = Random(mutant->position.x - patrolDestPosCorrectionValue, mutant->position.x + patrolDestPosCorrectionValue);
		mPatrolDestPos.z = Random(mutant->position.z - patrolDestPosCorrectionValue, mutant->position.z + patrolDestPosCorrectionValue);


		// ������ǥ������ ��ũ�⸦ ��� ��� ����ó��.
		float patrolRangeCorrectionValue = 30.0f;

		if (mPatrolDestPos.x >= mutant->GetTerrain()->GetSize().x)
		{
			mPatrolDestPos.x = mutant->GetTerrain()->GetSize().x - patrolRangeCorrectionValue;
		}
		else if (mPatrolDestPos.x <= 0.0f)
		{
			mPatrolDestPos.x = patrolRangeCorrectionValue;
		}

		if (mPatrolDestPos.z >= mutant->GetTerrain()->GetSize().y)
		{
			mPatrolDestPos.z = mutant->GetTerrain()->GetSize().y - patrolRangeCorrectionValue;
		}
		else if (mPatrolDestPos.z <= 0.0f)
		{
			mPatrolDestPos.z = patrolRangeCorrectionValue;
		}

		// ��μ���.
		mutant->GetPath().clear();
		mutant->GetPath().insert(mutant->GetPath().begin(), mPatrolDestPos);

		mbSetPatrolDest = false;
		mbPatrolMove = true;

		mDest = mutant->GetPath().back();

		mutant->RotateToDestination(mutant, mDest);
	}

	else if (!mbSetPatrolDest && mbPatrolMove) // �����̵�.
	{
		if (mutant->GetPath().empty())
			return;

		if (Timer::Get()->GetFPS() > 10)
		{
			mutant->MoveToDestination(mutant, mDest, mutant->GetMoveSpeed());
			mutant->SetAnimation(eAnimation::Run); // Run.
		}

		if (fabs(mPatrolDestPos.x - mutant->position.x) < 1.0f && // ��ǥ���� �����ϸ�
			fabs(mPatrolDestPos.z - mutant->position.z) < 1.0f)
		{
			mutant->GetPath().pop_back();
			mbPatrolMove = false;
			mPatrolRandomNum = GameMath::Random(5.0f, 10.0f); // ���� ����.
			mCurrentTime = Timer::Get()->GetRunTime();
			mStandTime = mCurrentTime + mPatrolRandomNum;

			mutant->SetAnimation(eAnimation::Idle);
		}
	}


	// ���� �� �÷��̾� �ִ��� üũ.
	mDistanceVector3ToPlayer = mPlayer->position - mutant->position;
	mDistanceToPlayer = sqrt(pow(mDistanceVector3ToPlayer.x, 2) + pow(mDistanceVector3ToPlayer.z, 2));

	if (mDistanceToPlayer <= mutant->GetDistanceBetweenPlayerAndMonsterForAttack()) // �÷��̾ �Ÿ� �ȿ� ������.
	{
		mutant->SetIsStalk(true);
		mutant->ChangeState(mutant->GetStalkingState()); // Stalking���� ������ȯ.
	}


}

void Patrol::Exit(Mutant* mutant)
{
}
