#include "Framework.h"


Warrok::Warrok()
	: 
	mAnimation(eAnimationStates::Idle),
	mFSM(eFSMstates::Patrol),
	mbOnHit(false)
{
	mScale = { 0.05f, 0.05f, 0.05f };

	SetMesh("Warrok", "Warrok.mesh");
	SetMaterial("Warrok", "Warrok.mat");
	SetShader(L"ModelAnimation");

	mRotation.y = XM_PI;
	UpdateWorld();
}

Warrok::~Warrok()
{

}

void Warrok::Update()
{
	mPosition.y = mTerrain->GetHeight(mPosition);

	UpdateWorld();
	ModelAnimator::Update();
}

void Warrok::InstanceUpdate()
{
}

void Warrok::PreRender()
{
}

void Warrok::Render()
{
	SetWorldBuffer();
	ModelAnimator::Render();
}

void Warrok::PostRender()
{
	
}

void Warrok::OnDamage(float damage)
{
	mFSM = eFSMstates::OnDamage;
	mbOnHit = true;
	GM->SetHitCheckMap(this, true);
	mCurrentHP -= 10.0f;
}

void Warrok::CheckOnHit()
{
	if (!mbOnHit) return;

	SetAnimation((eAnimationStates::OnDamage));
}

Collider* Warrok::GetColliderForAStar()
{
	return mBodyCollider;
}

void Warrok::setOnDamageEnd()
{
	SetAnimation((eAnimationStates::Idle));
	GM->SetHitCheckMap(this, false);
	mbOnHit = false;
}


void Warrok::SetIdle()
{
	SetAnimation((eAnimationStates::Idle));
}

void Warrok::SetAnimation(eAnimationStates value)
{
	if (mAnimation != value)
	{
		mAnimation = value;
		PlayClip(static_cast<int>(mAnimation));
	}
}



void Warrok::setAttackEnd()
{
	mPlayer = GM->Get()->GetPlayer();
	RotateToDestinationForModel(this, mPlayer->mPosition);
}

Collider* Warrok::GetHitCollider()
{
	return mBodyCollider;
}









