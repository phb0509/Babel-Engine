#include "Framework.h"

AttackState::AttackState()
{
}

AttackState::~AttackState()
{
}

void AttackState::Enter(Monster* monster)
{
}

void AttackState::Execute(Monster* monster)
{
	monster->SetAnimation(eAnimationStates::SmashAttack); // �⺻����.

	if (monster->GetDistanceToPlayer() > monster->GetDistanceToPlayerForAttack()) // ĳ���Ͱ� �־�����
	{
		monster->ChangeState(monster->GetStalkingState());
	}
}

void AttackState::Exit(Monster* mMonster)
{
} 
