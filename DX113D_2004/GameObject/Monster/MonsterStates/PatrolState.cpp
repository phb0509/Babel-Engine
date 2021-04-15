#include "Framework.h"

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



void PatrolState::Enter(Mutant* mutant)
{
	Initialize();
}

void PatrolState::Execute(Mutant* mutant)
{
	mPlayer = GM->Get()->GetPlayer();

	if (!mbPatrolMove) // 무브 끝난 이후 체크. 
	{
		if (Timer::Get()->GetRunTime() >= mStandTime)
		{
			mbSetPatrolDest = true;
		}
	}

	if (mbSetPatrolDest && !mbPatrolMove) // IDLE에서 일정 시간 지나면, 정찰모드. 목표지점 셋팅하고 그쪽방향으로 회전. PatrolState에서 맨 처음 실행.
	{
		// 정찰목표좌표 설정.
		float patrolDestPosCorrectionValue = 50.0f;
		mPatrolDestPos.x = Random(mutant->position.x - patrolDestPosCorrectionValue, mutant->position.x + patrolDestPosCorrectionValue);
		mPatrolDestPos.z = Random(mutant->position.z - patrolDestPosCorrectionValue, mutant->position.z + patrolDestPosCorrectionValue);


		// 정찰목표지점이 맵크기를 벗어날 경우 예외처리.
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

		// 경로설정.
		mutant->GetPath().clear();
		mutant->GetPath().insert(mutant->GetPath().begin(), mPatrolDestPos);

		mbSetPatrolDest = false;
		mbPatrolMove = true;

		mDest = mutant->GetPath().back();

		mutant->RotateToDestination(mutant, mDest);
	}

	else if (!mbSetPatrolDest && mbPatrolMove) // 실제이동.
	{
		if (mutant->GetPath().empty())
			return;

		
		if (Timer::Get()->GetFPS() > 10)
		{
			//mutant->MoveToDestination(mutant, mDest, mutant->GetMoveSpeed());
			mutant->MoveToDestUsingAStar(mDest);
			mutant->SetAnimation(eAnimation::Run); // Run.
		}

		if (fabs(mPatrolDestPos.x - mutant->position.x) < 1.0f && // 목표지점 도착하면
			fabs(mPatrolDestPos.z - mutant->position.z) < 1.0f)
		{
			mutant->GetPath().pop_back();
			mbPatrolMove = false;
			mPatrolRandomNum = GameMath::Random(5.0f, 10.0f); // 정찰 간격.
			mCurrentTime = Timer::Get()->GetRunTime();
			mStandTime = mCurrentTime + mPatrolRandomNum;

			mutant->SetAnimation(eAnimation::Idle);
		}
	}


	// 범위 안 플레이어 있는지 체크.
	if (mutant->GetDistanceToPlayer() <= mutant->GetPlayerDetectRange()) // 플레이어가 거리 안에 있으면.
	{
		mutant->SetIsStalk(true);
		mutant->ChangeState(mutant->GetStalkingState()); // Stalking으로 상태전환.
	}


}

void PatrolState::Exit(Mutant* mutant)
{
}
