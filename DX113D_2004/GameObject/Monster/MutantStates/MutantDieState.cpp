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
}

void MutantDieState::Execute(Monster* monster)
{
}

void MutantDieState::Exit(Monster* monster)
{
}
