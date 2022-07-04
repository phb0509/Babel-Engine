#include "Framework.h"

MutantDieState::MutantDieState()
{
}

MutantDieState::~MutantDieState()
{
}

void MutantDieState::Enter(Monster* monster)
{
	monster->SetFSMState(static_cast<int>(eMutantFSMStates::Die));
	monster->SetAnimation(static_cast<int>(eMutantAnimationStates::Die), true);
}

void MutantDieState::Execute(Monster* monster)
{
	if (monster->GetIsCompletedAnim()) // 피격모션 끝나면
	{
		monster->SetIsActive(false);
	}
}

void MutantDieState::Exit(Monster* monster)
{
}
