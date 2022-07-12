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

	monster->SetAnimation(static_cast<int>(eMutantAnimationStates::SmashAttack)); // �⺻����.

	if (monster->GetDistanceToPlayer() > monster->GetDistanceToPlayerForAttack()) // ĳ���Ͱ� �־�����
	{
		monster->ChangeState(monster->GetFSMState(static_cast<int>(eMutantFSMStates::Stalk)));
	}
}

void MutantAttackState::Exit(Monster* monster)
{
}
