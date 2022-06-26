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
	monster->SetAnimation(static_cast<int>(eMutantAnimationStates::SmashAttack)); // 기본공격.

	if (monster->GetDistanceToPlayer() > monster->GetDistanceToPlayerForAttack()) // 캐릭터가 멀어지면
	{
		//monster->ChangeState(InstanceMutant::GetStalkingState());
		monster->ChangeState(monster->GetState(static_cast<int>(eMutantFSMStates::Stalk)));
	}
}

void MutantAttackState::Exit(Monster* monster)
{
}
