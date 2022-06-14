#include "Framework.h"


InstanceMutant::InstanceMutant():
	mbOnHit(false)
{
	this->mScale = { 0.05f, 0.05f, 0.05f };

	mRotation.y = XM_PI;
	UpdateWorld();

	mPlayerDetectRange = 15.0f;
	mDistanceToPlayerForAttack = 7.0f;
	mCurrentState = GetPatrolState();
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

void InstanceMutant::CheckOnDamage(Collider* collider)
{
}

void InstanceMutant::CheckOnHit()
{
}

Collider* InstanceMutant::GetColliderForAStar()
{
	return nullptr;
}

void InstanceMutant::SetAnimation(eAnimationStates value) // State에서 SetAnimation하면 여기로 타고와서 실행되는데..
{
	if (mAnimationState != value)
	{
		mAnimationState = value;
		//PlayClip(static_cast<int>(mAnimationState));
		mUpperFrameBuffer->data.tweenDesc[mInstanceIndex].next.clip = static_cast<UINT>(value);
		mUpperFrameBuffer->data.tweenDesc[mInstanceIndex].next.speed = 1.0f;
		mUpperFrameBuffer->data.tweenDesc[mInstanceIndex].takeTime = 0.2f;
	}
}
