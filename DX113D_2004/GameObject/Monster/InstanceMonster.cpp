#include "Framework.h"


InstanceMonster::InstanceMonster():
	mbOnHit(false)
{
	this->mScale = { 0.05f, 0.05f, 0.05f };

	mRotation.y = XM_PI;
	UpdateWorld();

	mPlayerDetectRange = 15.0f;
	mDistanceToPlayerForAttack = 7.0f;
	mCurrentState = GetPatrolState();
}

InstanceMonster::~InstanceMonster()
{
}

void InstanceMonster::Update()
{
	mPosition.y = mTerrain->GetTargetPositionY(mPosition);

	mCurrentState->Execute(this);
	UpdateWorld();
}

void InstanceMonster::InstanceUpdate()
{
}

void InstanceMonster::PreRender()
{
}

void InstanceMonster::Render()
{

}

Collider* InstanceMonster::GetHitCollider()
{
	return nullptr;
}

void InstanceMonster::OnDamage(float damage)
{
}

void InstanceMonster::CheckOnHit()
{
}

Collider* InstanceMonster::GetColliderForAStar()
{
	return nullptr;
}

void InstanceMonster::SetAnimation(eAnimationStates value) // State���� SetAnimation�ϸ� ����� Ÿ��ͼ� ����Ǵµ�..
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
