#include "Framework.h"

MutantStalkingState::MutantStalkingState()
{
}

MutantStalkingState::~MutantStalkingState()
{
}

void MutantStalkingState::Enter(Monster* monster)
{
	mPlayer = GM->Get()->GetPlayer();
	monster->GetPath().clear();
	//mPeriodFuncPointer = bind(&Transform::RotateToDestinationForModel, monster, placeholders::_1, placeholders::_2);
}

void MutantStalkingState::Execute(Monster* monster)
{
	monster->MoveToDestUsingAStar(mPlayer->mPosition); // AStar Ÿ��
	monster->SetAnimation(static_cast<int>(eMutantAnimationStates::Run));

	if (monster->GetDistanceToPlayer() <= monster->GetDistanceToPlayerForAttack()) // ���� ��Ÿ� �ȿ� ������.
	{
		//monster->ChangeState(monster->GetAttackState()); // AttackState�� ��ȯ.
		monster->ChangeState(monster->GetState(static_cast<int>(eMutantFSMStates::Attack)));
	}
}

void MutantStalkingState::Exit(Monster* monster)
{

}
