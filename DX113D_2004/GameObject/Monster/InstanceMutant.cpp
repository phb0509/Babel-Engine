#include "Framework.h"


InstanceMutant::InstanceMutant(): // 디폴트체력 100.
	mbOnHit(false),
	mSmashAttackDamage(30.0f)
{
	this->mScale = { 0.05f, 0.05f, 0.05f };

	mRotation.y = XM_PI;
	UpdateWorld();

	mPlayerDetectRange = 15.0f;
	mDistanceToPlayerForAttack = 8.0f;

	mPatrolState = new MutantPatrolState();
	mStalkingState = new MutantStalkingState();
	mAttackState = new MutantAttackState();
	mDieState = new MutantDieState();
	mAttackedNormalState = new MutantAttackedNormalState();
	mAttackedKnockBackState = new MutantAttackedKnockBackState();

	mCurrentFSMState = mPatrolState;

	mStatusBar->SetMaxHP(mMaxHP);
	mStatusBar->SetCurHP(mCurHP);
	
	//SetEndEvent(static_cast<int>(eMutantFSMStates::AttackedNormal), bind(&InstanceMutant::setStalkingState, this));
}

InstanceMutant::~InstanceMutant()
{
	GM->SafeDelete(mPatrolState);
	GM->SafeDelete(mStalkingState);
	GM->SafeDelete(mAttackState);
	GM->SafeDelete(mDieState);
	GM->SafeDelete(mAttackedNormalState);
	GM->SafeDelete(mAttackedKnockBackState);
}

void InstanceMutant::Update()
{
	mPosition.y = mTerrain->GetTargetPositionY(mPosition);

	mCurrentFSMState->Execute(this);
	UpdateWorld();

	UIUpdate();
}

void InstanceMutant::InstanceUpdate()
{
}

void InstanceMutant::PreRender()
{
}

void InstanceMutant::Render()
{

}

Collider* InstanceMutant::GetHitCollider()
{
	return nullptr;
}

void InstanceMutant::CheckOnHit()
{
}

Collider* InstanceMutant::GetColliderForAStar()
{
	return nullptr;
}

int InstanceMutant::GetAnimationState()
{
	int temp = static_cast<int>(mCurrentAnimationState);
	return temp;
}

MonsterState* InstanceMutant::GetFSMState(int num)
{
	//Patrol,
	//Stalk,
	//Attack, 
	//AttackedNormal,
	//Die,
	//AttackedKnockBack

	switch (num)
	{
	case 0:
		return mPatrolState;
	case 1:
		return mStalkingState;
	case 2:
		return mAttackState;
	case 3:
		return mAttackedNormalState;
	case 4:
		return mDieState;
	case 5:
		return mAttackedKnockBackState;
	default:
		break;
	}
}

int InstanceMutant::GetEnumFSMState()
{
	return mCurrentEnumFSMState;
}

void InstanceMutant::SetFSMState(int state)
{
	mCurrentEnumFSMState = state;
}

void InstanceMutant::SetAnimation(int animationState, float speed, float takeTime, bool isForcingPlay) 
{
	eMutantAnimationStates state = static_cast<eMutantAnimationStates>(animationState);

	if (!isForcingPlay)
	{
		if (mCurrentAnimationState != state)
		{
			mCurrentAnimationState = state;
			mUpperFrameBuffer->data.tweenDesc[mInstanceIndex].next.clip = static_cast<UINT>(animationState);
			mUpperFrameBuffer->data.tweenDesc[mInstanceIndex].next.speed = speed;
			mUpperFrameBuffer->data.tweenDesc[mInstanceIndex].takeTime = takeTime;
		}
	}
	else
	{
		mCurrentAnimationState = state;
		mUpperFrameBuffer->data.tweenDesc[mInstanceIndex].next.clip = static_cast<UINT>(animationState);
		mUpperFrameBuffer->data.tweenDesc[mInstanceIndex].next.speed = speed;
		mUpperFrameBuffer->data.tweenDesc[mInstanceIndex].takeTime = takeTime;
	}
}

void InstanceMutant::ReActivation()
{
	ChangeState(GetFSMState(static_cast<int>(eMutantFSMStates::Patrol)));
	mbIsDie = false;
	mbIsActive = true;
	mCurHP = mMaxHP;
	mHPRate = mCurHP / mMaxHP;
	mStatusBar->SetCurHP(mCurHP);
	mStatusBar->SetHPRate(mHPRate);
}

void InstanceMutant::SetAttackInformations()
{
	AttackInformation attackInformation;
	attackInformation.attackColliders.push_back(mInstanceColliderData.collidersMap["smashAttackCollider"]);
	attackInformation.attackName = "SmashAttack";
	attackInformation.attackType = eAttackType::Normal;
	attackInformation.damage = mSmashAttackDamage;

	mAttackInformations["SmashAttack"] = attackInformation;
	
}

void InstanceMutant::OnDamage(AttackInformation attackInformation)
{
	mCurHP -= attackInformation.damage;
	
	if (mCurHP <= 0.0f) mCurHP = 0.0f;
	
	mHPRate = mCurHP / mMaxHP;
	mStatusBar->SetCurHP(mCurHP);
	mStatusBar->SetHPRate(mHPRate);
	
	if (mCurHP == 0.0f)
	{
		mbIsDie = true;
	}

	if (attackInformation.attackType == eAttackType::Normal)
	{
		GetFSMState(static_cast<int>(eMutantFSMStates::AttackedNormal))->SetIsDie(mbIsDie);
		GetFSMState(static_cast<int>(eMutantFSMStates::AttackedNormal))->SetAttackInformation(attackInformation);
		ChangeState(GetFSMState(static_cast<int>(eMutantFSMStates::AttackedNormal)));
	}
	else if (attackInformation.attackType == eAttackType::KnockBack)
	{
		GetFSMState(static_cast<int>(eMutantFSMStates::AttackedKnockBack))->SetIsDie(mbIsDie);
		GetFSMState(static_cast<int>(eMutantFSMStates::AttackedKnockBack))->SetAttackInformation(attackInformation);
		ChangeState(GetFSMState(static_cast<int>(eMutantFSMStates::AttackedKnockBack)));
	}
}

bool InstanceMutant::CheckIsCollision(Collider* collider)
{
	Collider* bodyCollider = mInstanceColliderData.collidersMap["bodyCollider"];

	if (bodyCollider->Collision(collider))
	{
		return true;
	}
	
	return false;
}

void InstanceMutant::setStalkingState()
{
	ChangeState(GetFSMState(static_cast<int>(eMutantFSMStates::Stalk)));
}

