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

	mCurrentState = mPatrolState;
}

InstanceMutant::~InstanceMutant()
{
}

void InstanceMutant::Update()
{
	mPosition.y = mTerrain->GetTargetPositionY(mPosition);

	mCurrentState->Execute(this);
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

void InstanceMutant::OnDamage(float damage)
{
}

bool InstanceMutant::CheckOnDamage(const Collider* collider)
{
	return false;
}

void InstanceMutant::CheckOnHit()
{
}

Collider* InstanceMutant::GetColliderForAStar()
{
	return nullptr;
}

int InstanceMutant::GetAnimationStates()
{
	int temp = static_cast<int>(mCurrentAnimationState);
	return temp;
}

MonsterState* InstanceMutant::GetState(int num)
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

void InstanceMutant::SetAnimation(int value) // 
{
	eMutantAnimationStates state = static_cast<eMutantAnimationStates>(value);

	if (mCurrentAnimationState != state)
	{
		mCurrentAnimationState = state;
		//PlayClip(static_cast<int>(mAnimationState));
		mUpperFrameBuffer->data.tweenDesc[mInstanceIndex].next.clip = static_cast<UINT>(value);
		mUpperFrameBuffer->data.tweenDesc[mInstanceIndex].next.speed = 1.0f;
		mUpperFrameBuffer->data.tweenDesc[mInstanceIndex].takeTime = 0.2f;
	}
}
