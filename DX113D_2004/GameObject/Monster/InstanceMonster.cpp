#include "Framework.h"


InstanceMonster::InstanceMonster()
{
	this->mScale = { 0.05f, 0.05f, 0.05f };
}

InstanceMonster::~InstanceMonster()
{
}

void InstanceMonster::Update()
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

void InstanceMonster::SetAnimation(eAnimationStates value)
{
}
