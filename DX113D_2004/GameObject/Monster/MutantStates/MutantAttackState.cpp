#include "Framework.h"

MutantAttackState::MutantAttackState():
	mbWasAttackedTarget(false)
{
}

MutantAttackState::~MutantAttackState()
{
}

void MutantAttackState::Enter(Monster* monster)
{
	mPlayer = GM->GetPlayer(); // Target
	map<string, AttackInformation> temp = monster->GetAttackInformations();
	mAttackInformation = temp["SmashAttack"];
	monster->SetFSMState(static_cast<int>(eMutantFSMStates::Attack));
}

void MutantAttackState::Execute(Monster* monster)
{
	if (monster->GetIsStartedAnim())
	{
		monster->RotateToDestinationForModel(monster, GM->GetPlayer()->mPosition);
	}

	if (monster->GetIsCompletedAnim()) // 공격모션 끝나면
	{
		mbWasAttackedTarget = false;
	}

	monster->SetAnimation(static_cast<int>(eMutantAnimationStates::SmashAttack)); // 기본공격.


	Collider* attackCollider = mAttackInformation.attackColliders[0];

	if (mPlayer->CheckIsCollision(attackCollider) && !mbWasAttackedTarget)
	{
		mPlayer->OnDamage(mAttackInformation);
		mbWasAttackedTarget = true;
	}

	if (monster->GetDistanceToPlayer() > monster->GetDistanceToPlayerForAttack()) // 캐릭터가 멀어지면
	{
		monster->ChangeState(monster->GetFSMState(static_cast<int>(eMutantFSMStates::Stalk)));
	}
}

void MutantAttackState::Exit(Monster* monster)
{
}
