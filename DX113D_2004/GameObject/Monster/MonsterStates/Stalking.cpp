#include "Framework.h"

Stalking::Stalking()
{
}

Stalking::~Stalking()
{
}

void Stalking::Enter(Mutant* mutant)
{
}

void Stalking::Execute(Mutant* mutant)
{
	mutant->RotateToDestination(mutant, GM->Get()->GetPlayer()->position);
	mutant->MoveToDestination(mutant, GM->Get()->GetPlayer()->position, mutant->GetMoveSpeed());
}

void Stalking::Exit(Mutant* mutant)
{
}
