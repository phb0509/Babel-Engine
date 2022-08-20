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
	monster->SetAnimation(static_cast<int>(eMutantAnimationStates::Die), 3.0f,0.2f,true);
}

void MutantDieState::Execute(Monster* monster)
{
	if (monster->GetIsCompletedAnim()) // 사망모션 끝나면
	{
		monster->SetLastPosDeactivation(monster->mPosition);
		monster->SetLastTimeDeactivation(TIME);
		monster->SetIsActive(false);
	}
}

void MutantDieState::Exit(Monster* monster)
{
	// 재활성화되는 경우.
}
