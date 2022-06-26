#include "Framework.h"

MutantAttackState::MutantAttackState()
{
}

MutantAttackState::~MutantAttackState()
{
}

void MutantAttackState::Enter(Monster* monster)
{
}

void MutantAttackState::Execute(Monster* monster)
{
	monster->SetAnimation(static_cast<int>(eMutantAnimationStates::SmashAttack)); // �⺻����.

	if (monster->GetDistanceToPlayer() > monster->GetDistanceToPlayerForAttack()) // ĳ���Ͱ� �־�����
	{
		//monster->ChangeState(InstanceMutant::GetStalkingState());
		monster->ChangeState(monster->GetState(static_cast<int>(eMutantFSMStates::Stalk)));
	}
}

void MutantAttackState::Exit(Monster* monster)
{
}
