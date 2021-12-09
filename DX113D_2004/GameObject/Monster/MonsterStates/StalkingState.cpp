#include "Framework.h"

StalkingState::StalkingState() :
	mbIsAttack(false)
{
}

StalkingState::~StalkingState()
{
}

void StalkingState::Enter(Monster* mMonster)
{
	mPlayer = GM->Get()->GetPlayer();
	mMonster->GetPath().clear();
	mPeriodFuncPointer = bind(&Transform::RotateToDestinationForModel, mMonster,placeholders::_1, placeholders::_2);
}

void StalkingState::Execute(Monster* mMonster)
{
	if (!mbIsAttack) // ��� Ÿ�� ����.
	{
		//mMonster->ExecuteRotationPeriodFunction(mPeriodFuncPointer, mMonster, mPlayer->position, 0.5f); // ����ȭ�� ���� �ݹ��Լ�... �����̼��ʿ����.
		//mutant->MoveToDestination(mutant, GM->Get()->GetPlayer()->position, mutant->GetMoveSpeed()); // �̵�

		mMonster->MoveToDestUsingAStar(GM->Get()->GetPlayer()->mPosition); // AStar Ÿ��
		mMonster->SetAnimation(eAnimationStates::Run);

		if (mMonster->GetDistanceToPlayer() <= mMonster->GetDistanceToPlayerForAttack()) // ���� ��Ÿ� �ȿ� ������.
		{
			mbIsAttack = true;
		}
	}

	else // ���� ����.
	{
		mMonster->SetAnimation(eAnimationStates::SmashAttack);

		if (mMonster->GetDistanceToPlayer() > mMonster->GetDistanceToPlayerForAttack()) // ĳ���Ͱ� �־�����
		{
			mbIsAttack = false;
		}

	}
	
}

void StalkingState::Exit(Monster* mMonster)
{

}
