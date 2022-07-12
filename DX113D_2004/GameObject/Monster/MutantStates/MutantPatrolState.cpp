#include "Framework.h"

MutantPatrolState::MutantPatrolState() :
	mPatrolRandomNum(0.0f),
	mbIsPatrolMove(false),
	mbIsSetPatrolDest(true),
	mCurrentTime(0.0f),
	mStandTime(0.0f),
	mPatrolDestPos(0.0f, 0.0f, 0.0f)
{
}

MutantPatrolState::~MutantPatrolState()
{
}

void MutantPatrolState::Initialize()
{
	mPatrolRandomNum = 0.0f;
	mbIsPatrolMove = false;
	mbIsSetPatrolDest = true;
	mCurrentTime = 0.0f;
	mStandTime = 0.0f;
	mPatrolDestPos = { 0.0f,0.0f,0.0f };
}

void MutantPatrolState::SetPatrolTargetPoint(Vector3& patrolTargetPoint)
{

}

void MutantPatrolState::Enter(Monster* monster)
{
	monster->SetFSMState(static_cast<int>(eMutantFSMStates::Patrol));
	Initialize();
}

void MutantPatrolState::Execute(Monster* monster)
{
	mPlayer = GM->Get()->GetPlayer();
	mMaxPatrolWidth = monster->GetTerrain()->GetSize().x - 1;
	mMaxPatrolHeight = monster->GetTerrain()->GetSize().y - 1;

	if (!mbIsPatrolMove) // 무브 끝난 이후 체크. 
	{
		if (Timer::Get()->GetRunTime() >= mStandTime)
		{
			mbIsSetPatrolDest = true; // 정찰목표좌표설정을 실행해야한다는 표시.
		}
	}

	if (mbIsSetPatrolDest && !mbIsPatrolMove) // IDLE에서 일정 시간 지나면, 정찰모드. 목표지점 셋팅하고 그쪽방향으로 회전. PatrolState에서 맨 처음 실행.
	{
		// 정찰목표좌표 설정.
		float patrolDestPosCorrectionValue = 50.0f; // 정찰범위값
		mPatrolDestPos.x = Random(monster->mPosition.x - patrolDestPosCorrectionValue, monster->mPosition.x + patrolDestPosCorrectionValue);
		mPatrolDestPos.z = Random(monster->mPosition.z - patrolDestPosCorrectionValue, monster->mPosition.z + patrolDestPosCorrectionValue);

		mbIsSetPatrolDest = false; // 정찰목표좌표설정 해줬으니 false.
		mbIsPatrolMove = true; // 실제 이동해야하니 true.

		// 정찰목표지점이 맵크기를 벗어날 경우 예외처리.
		float patrolRangeCorrectionValue = 30.0f;

		if (mPatrolDestPos.x >= mMaxPatrolWidth)
		{
			mPatrolDestPos.x = mMaxPatrolWidth - patrolRangeCorrectionValue;
		}
		else if (mPatrolDestPos.x <= 0.0f)
		{
			mPatrolDestPos.x = patrolRangeCorrectionValue;
		}

		if (mPatrolDestPos.z >= mMaxPatrolHeight)
		{
			mPatrolDestPos.z = mMaxPatrolHeight - patrolRangeCorrectionValue;
		}
		else if (mPatrolDestPos.z <= 0.0f)
		{
			mPatrolDestPos.z = patrolRangeCorrectionValue;
		}

		//mbIsSetPatrolDest = false;
		//mbIsPatrolMove = true;

		mPatrolDestPos = monster->GetAStar()->FindCloseNodePosition(mPatrolDestPos); // 목표좌표에서 가장 가까운 노드위치 리턴.그런데 굳이??

		//monster->GetPath().clear(); // SetAStarPath에서 맨처음에 수행해줌. 
		monster->SetAStarPath(mPatrolDestPos);
		monster->RotateToDestinationForModel(monster, monster->GetPath().back());
	}

	else if (!mbIsSetPatrolDest && mbIsPatrolMove) // 실제이동.
	{
		if (Timer::Get()->GetFPS() > 10) // 초기실행시 FPS값이 순간적으로 1이 되는것에 따른 버그로 인한 예외처리. 
		{
			//mMonster->MoveToDestUsingAStar(mPatrolDestPos);

			float length = (monster->GetPath().back() - monster->mPosition).Length();

			if (length < 1.0f) // 노드에 도착하면
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

			monster->SetAnimation(static_cast<int>(eMutantAnimationStates::Run)); // Run.
		}

		if (fabs(mPatrolDestPos.x - monster->mPosition.x) < 1.0f && // 목표지점 도착하면
			fabs(mPatrolDestPos.z - monster->mPosition.z) < 1.0f)
		{
			mbIsPatrolMove = false;
			mPatrolRandomNum = GameMath::Random(5.0f, 10.0f); // 정찰 간격.
			mCurrentTime = TIME;
			mStandTime = mCurrentTime + mPatrolRandomNum;

			monster->SetAnimation(static_cast<int>(eMutantAnimationStates::Idle));
		}
	}

	//범위 안 플레이어 있는지 체크.
	if (monster->GetDistanceToPlayer() <= monster->GetPlayerDetectRange()) // 플레이어가 거리 안에 있으면.
	{
		monster->ChangeState(monster->GetFSMState(static_cast<int>(eMutantFSMStates::Stalk)));
	}
}

void MutantPatrolState::Exit(Monster* mMonster)
{
}
