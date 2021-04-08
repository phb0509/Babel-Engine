#include "Framework.h"

Stalking::Stalking()
{
}

Stalking::~Stalking()
{
}

void Stalking::Enter(Mutant* mutant)
{
	mPlayer = GM->Get()->GetPlayer();
	mPeriodFuncPointer = bind(&Transform::RotateToDestination, mutant, placeholders::_1, placeholders::_2);
}

void Stalking::Execute(Mutant* mutant)
{
	mutant->ExecutePeriodFunction(mPeriodFuncPointer, mutant, mPlayer->position, 0.5f);
	mutant->MoveToDestination(mutant, GM->Get()->GetPlayer()->position, mutant->GetMoveSpeed());

	mutant->SetAnimation(eAnimation::Run);
}

void Stalking::Exit(Mutant* mutant)
{

}
