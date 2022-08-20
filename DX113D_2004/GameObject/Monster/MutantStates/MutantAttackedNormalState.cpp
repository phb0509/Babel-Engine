#include "Framework.h"

MutantAttackedNormalState::MutantAttackedNormalState()
{
}

MutantAttackedNormalState::~MutantAttackedNormalState()
{
}

void MutantAttackedNormalState::Enter(Monster* monster)
{
	monster->SetFSMState(static_cast<int>(eMutantFSMStates::AttackedNormal));
	monster->SetAnimation(static_cast<int>(eMutantAnimationStates::AttackedNormal),1.0f,0.2f, true);
}

void MutantAttackedNormalState::Execute(Monster* monster)
{
	if (mbIsDie)
	{
		monster->ChangeState(monster->GetFSMState(static_cast<int>(eMutantFSMStates::Die)));
		mbIsDie = true;
	}

	if (monster->GetIsCompletedAnim()) // 피격모션 끝나면
	{
		monster->ChangeState(monster->GetFSMState(static_cast<int>(eMutantFSMStates::Stalk)));
	}
}

void MutantAttackedNormalState::Exit(Monster* monster)
{
}
