#include "Framework.h"

StalkingState::StalkingState()
{
}

StalkingState::~StalkingState()
{
}

void StalkingState::Enter(Monster* monster)
{
	mPlayer = GM->Get()->GetPlayer();
	monster->GetPath().clear();
	//mPeriodFuncPointer = bind(&Transform::RotateToDestinationForModel, monster, placeholders::_1, placeholders::_2);
}

void StalkingState::Execute(Monster* monster)
{
	monster->MoveToDestUsingAStar(mPlayer->mPosition); // AStar 타겟
	monster->SetAnimation(eAnimationStates::Run);

	if (monster->GetDistanceToPlayer() <= monster->GetDistanceToPlayerForAttack()) // 공격 사거리 안에 들어오면.
	{
		monster->ChangeState(monster->GetAttackState()); // AttackState로 전환.
	}
}

void StalkingState::Exit(Monster* monster)
{

}
