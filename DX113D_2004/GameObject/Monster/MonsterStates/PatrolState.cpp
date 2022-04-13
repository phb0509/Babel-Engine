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
		mPatrolDestPos.x = Random(mMonster->mPosition.x - patrolDestPosCorrectionValue, mMonster->mPosition.x + patrolDestPosCorrectionValue);
		mPatrolDestPos.z = Random(mMonster->mPosition.z - patrolDestPosCorrectionValue, mMonster->mPosition.z + patrolDestPosCorrectionValue);

		// 정찰목표지점이 맵크기를 벗어날 경우 예외처리.
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

	else if (!mbSetPatrolDest && mbPatrolMove) // 실제이동.
	{		
		if (Timer::Get()->GetFPS() > 10) // 초기실행시 FPS값이 순간적으로 1이 되는것에 따른 버그로 인한 예외처리. 
		{
			//mMonster->MoveToDestUsingAStar(mPatrolDestPos);

			float length = (mMonster->GetPath().back() - mMonster->mPosition).Length();

			if (length < 1.0f) // 노드에 도착하면
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

		if (fabs(mPatrolDestPos.x - mMonster->mPosition.x) < 1.0f && // 목표지점 도착하면
			fabs(mPatrolDestPos.z - mMonster->mPosition.z) < 1.0f)
		{
			mbPatrolMove = false;
			mPatrolRandomNum = GameMath::Random(5.0f, 10.0f); // 정찰 간격.
			mCurrentTime = Timer::Get()->GetRunTime();
			mStandTime = mCurrentTime + mPatrolRandomNum;

			mMonster->SetAnimation(eAnimationStates::Idle);
		}
	}

	//범위 안 플레이어 있는지 체크.
	if (mMonster->GetDistanceToPlayer() <= mMonster->GetPlayerDetectRange()) // 플레이어가 거리 안에 있으면.
	{
		mMonster->SetIsStalk(true);
		mMonster->ChangeState(mMonster->GetStalkingState()); // Stalking으로 상태전환.
	}
}

void PatrolState::Exit(Monster* mMonster)
{
}
