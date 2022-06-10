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
	monster->SetAnimation(eAnimationStates::SmashAttack); // 기본공격.

	if (monster->GetDistanceToPlayer() > monster->GetDistanceToPlayerForAttack()) // 캐릭터가 멀어지면
	{
		monster->ChangeState(monster->GetStalkingState());
	}
}

void AttackState::Exit(Monster* mMonster)
{
} 
