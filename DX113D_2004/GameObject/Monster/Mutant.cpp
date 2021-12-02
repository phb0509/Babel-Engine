#include "Framework.h"


Mutant::Mutant()
	: ModelAnimator(),
	mAnimation(eAnimationStates::Idle),
	mFSM(eStates::Patrol),
	mbOnHit(false)
{
	mScale = { 0.05f, 0.05f, 0.05f };

	SetMesh("Mutant", "Mutant.mesh");
	SetMaterial("Mutant", "Mutant.mat");

	SetShader(L"ModelAnimation");

	ReadClip("Mutant/Idle0.clip");
	ReadClip("Mutant/Run0.clip");
	ReadClip("Mutant/SmashAttack0.clip");
	ReadClip("Mutant/OnDamage0.clip");
	ReadClip("Mutant/Die0.clip");

	SetEndEvent(static_cast<int>(eAnimationStates::Run), bind(&Mutant::SetIdle, this));
	SetEndEvent(static_cast<int>(eAnimationStates::OnDamage), bind(&Mutant::setOnDamageEnd, this));
	SetEndEvent(static_cast<int>(eAnimationStates::SmashAttack), bind(&Mutant::setAttackEnd, this));
	
	PlayClip(0);

	mBodyCollider = new BoxCollider();
	mSmashAttackCollider = new BoxCollider();
	loadCollider(); // 툴에서 셋팅한 컬라이더 불러오기.

	mRotation.y = XM_PI;
	UpdateWorld();

	mPlayerDetectRange = 15.0f;
	mDistanceToPlayerForAttack = 7.0f;
    mCurrentState = GetPatrolState();
}

Mutant::~Mutant()
{
	delete mBodyCollider;
	delete mSmashAttackCollider;
}

void Mutant::Update()
{
	setColliders();

	mPosition.y = mTerrain->GetHeight(mPosition);

	mBodyCollider->Update();
	mSmashAttackCollider->Update();

	mCurrentState->Execute(this);

	UpdateWorld();
	ModelAnimator::Update();
}

void Mutant::Render()
{
	mBodyCollider->Render();
	mSmashAttackCollider->Render();

	SetWorldBuffer();
	ModelAnimator::Render();
}

void Mutant::PostRender()
{
	//ImGui::SliderFloat3("Pos", (float*)&weapon->position, -30, 30);
	//ImGui::SliderFloat3("Rot", (float*)&weapon->rotation, 0, XM_2PI);
	//ImGui::SliderFloat3("Scale", (float*)&weapon->scale, -30, 30);
}

void Mutant::OnDamage(float damage)
{
	mFSM = eStates::OnDamage;
	mbOnHit = true;
	GM->SetHitCheckMap(this, true);
	mCurrentHP -= 10.0f;
}

void Mutant::CheckOnHit()
{
	if (!mbOnHit) return;

	SetAnimation((eAnimationStates::OnDamage));
}

Collider* Mutant::GetColliderForAStar()
{
	return mBodyCollider;
}

void Mutant::setOnDamageEnd()
{
	SetAnimation((eAnimationStates::Idle));
	GM->SetHitCheckMap(this, false);
	mbOnHit = false;
}


void Mutant::SetIdle()
{
	SetAnimation((eAnimationStates::Idle));
}

void Mutant::SetAnimation(eAnimationStates value)
{
	if (mAnimation != value)
	{
		mAnimation = value;
		PlayClip(static_cast<int>(mAnimation));
	}
}



void Mutant::setAttackEnd()
{
	mPlayer = GM->Get()->GetPlayer();
	RotateToDestinationForModel(this, mPlayer->mPosition);
}

Collider* Mutant::GetHitCollider()
{
	return mBodyCollider;
}


void Mutant::loadCollider()
{
	BinaryReader colliderReader(L"TextData/Mutant.map");
	UINT colliderSize = colliderReader.UInt();

	mTempColliderDatas.resize(colliderSize);
	mColliderDatas.resize(colliderSize);


	void* ptr1 = (void*)mTempColliderDatas.data();

	for (int i = 0; i < colliderSize; i++) // 툴에서 저장한 컬라이더이름.
	{
		mColliderDatas[i].colliderName = colliderReader.String();
		mColliderDatas[i].nodeName = colliderReader.String();
	}

	colliderReader.Byte(&ptr1, sizeof(temp_colliderData) * colliderSize);

	for (int i = 0; i < colliderSize; i++)
	{
		mColliderDatas[i].position = mTempColliderDatas[i].position;
		mColliderDatas[i].rotation = mTempColliderDatas[i].rotation;
		mColliderDatas[i].scale = mTempColliderDatas[i].scale;
	}


	findCollider("smashAttackCollider", mSmashAttackCollider);
	findCollider("BodyCollider", mBodyCollider); // BodyCollider의 SRT값을 mBodyCollider에.
}

void Mutant::findCollider(string name, Collider* collider)
{
	for (int i = 0; i < mColliderDatas.size(); i++)
	{
		if (mColliderDatas[i].colliderName == name)
		{
			collider->mPosition = mColliderDatas[i].position;
			collider->mRotation = mColliderDatas[i].rotation;
			collider->mScale = mColliderDatas[i].scale;
		}
	}
}


void Mutant::setColliders()
{
	int bodyIndex = GetNodeByName("Mutant:Spine");
	mBodyMatrix = GetTransformByNode(bodyIndex) * mWorldMatrix;
	mBodyCollider->SetParent(&mBodyMatrix);

	int smashAttackIndex = GetNodeByName("Mutant:LeftHand");
	mSmashAttackMatrix = GetTransformByNode(smashAttackIndex) * mWorldMatrix;
	mSmashAttackCollider->SetParent(&mSmashAttackMatrix);
}




