#include "Framework.h"

StalkingState::StalkingState() :
	mbIsAttack(false)
{
}

StalkingState::~StalkingState()
{
}

void StalkingState::Enter(Monster* monster)
{
	mPlayer = GM->Get()->GetPlayer();
	mPeriodFuncPointer = bind(&Transform::RotateToDestination, monster,placeholders::_1, placeholders::_2);
}

void StalkingState::Execute(Monster* monster)
{
	if (!mbIsAttack) // ��� Ÿ�� ����.
	{
		//monster->ExecuteRotationPeriodFunction(mPeriodFuncPointer, monster, mPlayer->position, 0.5f); // ����ȭ�� ���� �ݹ��Լ�... �����̼��ʿ����.
		//mutant->MoveToDestination(mutant, GM->Get()->GetPlayer()->position, mutant->GetMoveSpeed()); // �̵�

		monster->MoveToDestUsingAStar(GM->Get()->GetPlayer()->position);
		monster->SetAnimation(eAnimation::Run);

		if (monster->GetDistanceToPlayer() <= monster->GetDistanceToPlayerForAttack()) // ���� ��Ÿ� �ȿ� ������.
		{
			mbIsAttack = true;
		}
	}

	else // ���� ����.
	{
		monster->SetAnimation(eAnimation::SmashAttack);

		if (monster->GetDistanceToPlayer() > monster->GetDistanceToPlayerForAttack())
		{
			mbIsAttack = false;
		}
	}
	
}

void StalkingState::Exit(Monster* monster)
{

}
