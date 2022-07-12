#include "Framework.h"

MutantAttackState::MutantAttackState()
{
}

MutantAttackState::~MutantAttackState()
{
}

void MutantAttackState::Enter(Monster* monster)
{
	monster->SetFSMState(static_cast<int>(eMutantFSMStates::Attack));
}

void MutantAttackState::Execute(Monster* monster)
{
	if (monster->GetIsStartedAnim())
	{
		monster->RotateToDestinationForModel(monster, GM->GetPlayer()->mPosition);
	}

	monster->SetAnimation(static_cast<int>(eMutantAnimationStates::SmashAttack)); // 기본공격.

	if (monster->GetDistanceToPlayer() > monster->GetDistanceToPlayerForAttack()) // 캐릭터가 멀어지면
	{
		monster->ChangeState(monster->GetFSMState(static_cast<int>(eMutantFSMStates::Stalk)));
	}
}

void MutantAttackState::Exit(Monster* monster)
{
}
