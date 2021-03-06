#include "Framework.h"

MutantStalkingState::MutantStalkingState()
{
}

MutantStalkingState::~MutantStalkingState()
{
}

void MutantStalkingState::Enter(Monster* monster)
{
	monster->SetFSMState(static_cast<int>(eMutantFSMStates::Stalk));
	mPlayer = GM->Get()->GetPlayer();
	monster->GetPath().clear();
	//mPeriodFuncPointer = bind(&Transform::RotateToDestinationForModel, monster, placeholders::_1, placeholders::_2);
}

void MutantStalkingState::Execute(Monster* monster)
{
	monster->MoveToDestUsingAStar(mPlayer->mPosition); // AStar 타겟
	monster->SetAnimation(static_cast<int>(eMutantAnimationStates::Run));

	if (monster->GetDistanceToPlayer() <= monster->GetDistanceToPlayerForAttack()) // 공격 사거리 안에 들어오면.
	{
		monster->ChangeState(monster->GetFSMState(static_cast<int>(eMutantFSMStates::Attack)));
	}
}

void MutantStalkingState::Exit(Monster* monster)
{

}
