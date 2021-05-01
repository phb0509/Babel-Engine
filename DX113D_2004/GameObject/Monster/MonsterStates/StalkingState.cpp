#include "Framework.h"

StalkingState::StalkingState() :
	mbIsAttack(false)
{
}

StalkingState::~StalkingState()
{
}

void StalkingState::Enter(Monster* monster)
{
	mPlayer = GM->Get()->GetPlayer();
	monster->GetPath().clear();
	mPeriodFuncPointer = bind(&Transform::RotateToDestination, monster,placeholders::_1, placeholders::_2);
}

void StalkingState::Execute(Monster* monster)
{
	if (!mbIsAttack) // 계속 타겟 추적.
	{
		//monster->ExecuteRotationPeriodFunction(mPeriodFuncPointer, monster, mPlayer->position, 0.5f); // 최적화를 위한 콜백함수... 로테이션필요없다.
		//mutant->MoveToDestination(mutant, GM->Get()->GetPlayer()->position, mutant->GetMoveSpeed()); // 이동

		monster->MoveToDestUsingAStar(GM->Get()->GetPlayer()->position); // AStar 타겟
		monster->SetAnimation(eAnimationStates::Run);

		if (monster->GetDistanceToPlayer() <= monster->GetDistanceToPlayerForAttack()) // 공격 사거리 안에 들어오면.
		{
			mbIsAttack = true;
		}
	}

	else // 공격 상태.
	{
		monster->SetAnimation(eAnimationStates::SmashAttack);

		if (monster->GetDistanceToPlayer() > monster->GetDistanceToPlayerForAttack()) // 캐릭터가 멀어지면
		{
			mbIsAttack = false;
		}

	}
	
}

void StalkingState::Exit(Monster* monster)
{

}
