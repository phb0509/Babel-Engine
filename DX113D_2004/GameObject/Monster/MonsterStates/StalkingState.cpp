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
		mutant->ExecuteRotationPeriodFunction(mPeriodFuncPointer, mutant, mPlayer->position, 0.5f); // ����ȭ�� ���� �ݹ��Լ�.
		//mutant->MoveToDestination(mutant, GM->Get()->GetPlayer()->position, mutant->GetMoveSpeed()); // �̵�
		mutant->MoveToDestUsingAStar(GM->Get()->GetPlayer()->position);
		mutant->SetAnimation(eAnimation::Run);

		if (mutant->GetDistanceToPlayer() <= mutant->GetDistanceToPlayerForAttack()) // ���� ��Ÿ� �ȿ� ������.
		{
			mbIsAttack = true;
		}
	}
	else // ���� ����.
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
