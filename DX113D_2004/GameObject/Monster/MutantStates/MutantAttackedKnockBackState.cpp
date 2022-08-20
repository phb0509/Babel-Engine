#include "Framework.h"

MutantAttackedKnockBackState::MutantAttackedKnockBackState() :
	mKnockBackSpeed(30.0f),
	mReductionSpeed(30.0f)
{
}

MutantAttackedKnockBackState::~MutantAttackedKnockBackState()
{
}

void MutantAttackedKnockBackState::Enter(Monster* monster)
{
	monster->SetFSMState(static_cast<int>(eMutantFSMStates::AttackedKnockBack));
	monster->SetAnimation(static_cast<int>(eMutantAnimationStates::AttackedKnockBack), 1.0f, 0.2f, true);
}

void MutantAttackedKnockBackState::Execute(Monster* monster)
{
	if (mbIsDie)
	{
		monster->ChangeState(monster->GetFSMState(static_cast<int>(eMutantFSMStates::Die)));
		mbIsDie = true;
	}

	if (monster->GetIsCompletedAnim()) // 피격모션 끝나면
	{
		monster->ChangeState(monster->GetFSMState(static_cast<int>(eMutantFSMStates::Stalk)));
		mKnockBackSpeed = 30.0f;
	}
	else
	{
		monster->mPosition += monster->GetForwardVector() * mKnockBackSpeed * DELTA;
		mKnockBackSpeed -= mReductionSpeed * DELTA;

		if (mKnockBackSpeed <= 0.0f)
		{
			mKnockBackSpeed = 0.0f;
		}
	}
}

void MutantAttackedKnockBackState::Exit(Monster* monster)
{
	
}
