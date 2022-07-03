#include "Framework.h"

MutantOnDamageState::MutantOnDamageState()
{
}

MutantOnDamageState::~MutantOnDamageState()
{
}

void MutantOnDamageState::Enter(Monster* monster)
{
	monster->SetFSMState(static_cast<int>(eMutantFSMStates::OnDamage));
	monster->SetAnimation(static_cast<int>(eMutantAnimationStates::OnDamage),true);
}

void MutantOnDamageState::Execute(Monster* monster)
{
	if (monster->GetIsCompletedAnim()) // �ǰݸ�� ������
	{
		monster->ChangeState(monster->GetFSMState(static_cast<int>(eMutantFSMStates::Stalk)));
	}
}

void MutantOnDamageState::Exit(Monster* monster)
{
}
