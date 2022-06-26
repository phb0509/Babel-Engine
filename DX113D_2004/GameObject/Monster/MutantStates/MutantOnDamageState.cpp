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
}

void MutantOnDamageState::Execute(Monster* monster)
{
}

void MutantOnDamageState::Exit(Monster* monster)
{
}
