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
	monster->MoveToDestUsingAStar(mPlayer->mPosition); // AStar Ÿ��
	monster->SetAnimation(eAnimationStates::Run);

	if (monster->GetDistanceToPlayer() <= monster->GetDistanceToPlayerForAttack()) // ���� ��Ÿ� �ȿ� ������.
	{
		monster->ChangeState(monster->GetAttackState()); // AttackState�� ��ȯ.
	}
}

void StalkingState::Exit(Monster* monster)
{

}
