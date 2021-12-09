#include "Framework.h"

StalkingState::StalkingState() :
	mbIsAttack(false)
{
}

StalkingState::~StalkingState()
{
}

void StalkingState::Enter(Monster* mMonster)
{
	mPlayer = GM->Get()->GetPlayer();
	mMonster->GetPath().clear();
	mPeriodFuncPointer = bind(&Transform::RotateToDestinationForModel, mMonster,placeholders::_1, placeholders::_2);
}

void StalkingState::Execute(Monster* mMonster)
{
	if (!mbIsAttack) // 계속 타겟 추적.
	{
		//mMonster->ExecuteRotationPeriodFunction(mPeriodFuncPointer, mMonster, mPlayer->position, 0.5f); // 최적화를 위한 콜백함수... 로테이션필요없다.
		//mutant->MoveToDestination(mutant, GM->Get()->GetPlayer()->position, mutant->GetMoveSpeed()); // 이동

		mMonster->MoveToDestUsingAStar(GM->Get()->GetPlayer()->mPosition); // AStar 타겟
		mMonster->SetAnimation(eAnimationStates::Run);

		if (mMonster->GetDistanceToPlayer() <= mMonster->GetDistanceToPlayerForAttack()) // 공격 사거리 안에 들어오면.
		{
			mbIsAttack = true;
		}
	}

	else // 공격 상태.
	{
		mMonster->SetAnimation(eAnimationStates::SmashAttack);

		if (mMonster->GetDistanceToPlayer() > mMonster->GetDistanceToPlayerForAttack()) // 캐릭터가 멀어지면
		{
			mbIsAttack = false;
		}

	}
	
}

void StalkingState::Exit(Monster* mMonster)
{

}
