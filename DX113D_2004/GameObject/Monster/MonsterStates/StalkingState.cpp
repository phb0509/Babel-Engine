#include "Framework.h"

StalkingState::StalkingState() :
	mbIsAttack(false)
{
}

StalkingState::~StalkingState()
{
}

void StalkingState::Enter(Mutant* mutant)
{
	mPlayer = GM->Get()->GetPlayer();
	mPeriodFuncPointer = bind(&Transform::RotateToDestination, mutant,placeholders::_1, placeholders::_2);
}

void StalkingState::Execute(Mutant* mutant)
{
	if (!mbIsAttack)
	{
		mutant->ExecuteRotationPeriodFunction(mPeriodFuncPointer, mutant, mPlayer->position, 0.5f); // 최적화를 위한 콜백함수.
		//mutant->MoveToDestination(mutant, GM->Get()->GetPlayer()->position, mutant->GetMoveSpeed()); // 이동
		mutant->MoveToDestUsingAStar(GM->Get()->GetPlayer()->position);
		mutant->SetAnimation(eAnimation::Run);

		if (mutant->GetDistanceToPlayer() <= mutant->GetDistanceToPlayerForAttack()) // 공격 사거리 안에 들어오면.
		{
			mbIsAttack = true;
		}
	}
	else // 공격 상태.
	{
		mutant->SetAnimation(eAnimation::SmashAttack);

		if (mutant->GetDistanceToPlayer() > mutant->GetDistanceToPlayerForAttack())
		{
			mbIsAttack = false;
		}
	}
	
}

void StalkingState::Exit(Mutant* mutant)
{

}
