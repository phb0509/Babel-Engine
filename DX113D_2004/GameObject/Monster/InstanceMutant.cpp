#include "Framework.h"


InstanceMutant::InstanceMutant():
	mbOnHit(false)
{
	this->mScale = { 0.05f, 0.05f, 0.05f };

	mRotation.y = XM_PI;
	UpdateWorld();

	mPlayerDetectRange = 15.0f;
	mDistanceToPlayerForAttack = 7.0f;

	mPatrolState = new MutantPatrolState();
	mStalkingState = new MutantStalkingState();
	mAttackState = new MutantAttackState();
	mOnDamageState = new MutantOnDamageState();
	mDieState = new MutantDieState();

	mCurrentFSMState = mPatrolState;

	//SetEndEvent(static_cast<int>(eMutantFSMStates::OnDamage), bind(&InstanceMutant::setStalkingState, this));
}

InstanceMutant::~InstanceMutant()
{
	GM->SafeDelete(mPatrolState);
	GM->SafeDelete(mStalkingState);
	GM->SafeDelete(mAttackState);
	GM->SafeDelete(mOnDamageState);
	GM->SafeDelete(mDieState);
}

void InstanceMutant::Update()
{
	mPosition.y = mTerrain->GetTargetPositionY(mPosition);

	mCurrentFSMState->Execute(this);
	UpdateWorld();
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
	//OnDamage,
	//Die,

	switch (num)
	{
	case 0:
		return mPatrolState;
	case 1:
		return mStalkingState;
	case 2:
		return mAttackState;
	case 3:
		return mOnDamageState;
	case 4:
		return mDieState;
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
}

void InstanceMutant::OnDamage(AttackInformation attackInformation)
{
	mCurHP -= attackInformation.damage;

	if (mCurHP <= 0)
	{
		ChangeState(GetFSMState(static_cast<int>(eMutantFSMStates::Die)));
		mbIsDie = true;
	}
	else
	{
		ChangeState(GetFSMState(static_cast<int>(eMutantFSMStates::OnDamage)));
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

