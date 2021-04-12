#include "Framework.h"


Mutant::Mutant()
	: ModelAnimator("Mutant/Mutant"),
	mAnimation(eAnimation::Idle),
	mFSM(eFSM::Patrol),
	mbOnHit(false)
{
	scale = { 0.05f, 0.05f, 0.05f };

	SetShader(L"ModelAnimation");

	ReadClip("Mutant/Idle0.clip");
	ReadClip("Mutant/Run0.clip");
	ReadClip("Mutant/SmashAttack0.clip");
	ReadClip("Mutant/OnDamage0.clip");
	ReadClip("Mutant/Die0.clip");

	SetEndEvent(static_cast<int>(eAnimation::Run), bind(&Mutant::SetIdle, this));
	SetEndEvent(static_cast<int>(eAnimation::OnDamage), bind(&Mutant::setOnDamageEnd, this));
	SetEndEvent(static_cast<int>(eAnimation::SmashAttack), bind(&Mutant::setAttackEnd, this));
	
	PlayClip(0);

	mBodyCollider = new BoxCollider();
	mSmashAttackCollider = new BoxCollider();
	loadCollider(); // 툴에서 셋팅한 컬라이더 불러오기.

	rotation.y = XM_PI;
	UpdateWorld();

	mPlayerDetectRange = 40.0f;
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

	position.y = mTerrain->GetHeight(position);

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
	mFSM = eFSM::OnDamage;
	mbOnHit = true;
	GM->SetHitCheckMap(this, true);
	mCurrentHP -= 10.0f;
}

void Mutant::CheckOnHit()
{
	if (!mbOnHit) return;

	SetAnimation((eAnimation::OnDamage));
}

void Mutant::ChangeState(State* nextState)
{
	//static_assert(mCurrentState && nextState);

	mCurrentState->Exit(this);

	mCurrentState = nextState;

	mCurrentState->Enter(this);
}




void Mutant::setOnDamageEnd()
{
	SetAnimation((eAnimation::Idle));
	GM->SetHitCheckMap(this, false);
	mbOnHit = false;
}


void Mutant::SetIdle()
{
	SetAnimation((eAnimation::Idle));
}

void Mutant::SetAnimation(eAnimation value)
{
	if (mAnimation != value)
	{
		mAnimation = value;
		PlayClip(static_cast<int>(mAnimation));
	}
}

void Mutant::setColliders()
{
	int bodyIndex = GetNodeByName("Mutant:Spine");
	mBodyMatrix = GetTransformByNode(bodyIndex) * world;
	mBodyCollider->SetParent(&mBodyMatrix);

	int smashAttackIndex = GetNodeByName("Mutant:LeftHand");
	mSmashAttackMatrix = GetTransformByNode(smashAttackIndex) * world;
	mSmashAttackCollider->SetParent(&mSmashAttackMatrix);
}

void Mutant::setAttackEnd()
{
	mPlayer = GM->Get()->GetPlayer();
	RotateToDestination(this, mPlayer->position);
}

void Mutant::loadCollider()
{
	BinaryReader colliderReader(L"TextData/Mutant.map");
	UINT colliderSize = colliderReader.UInt();

	mTempColliderDatas.resize(colliderSize);
	mColliderDatas.resize(colliderSize);


	void* ptr1 = (void*)mTempColliderDatas.data();

	for (int i = 0; i < colliderSize; i++)
	{
		mColliderDatas[i].name = colliderReader.String();
	}

	colliderReader.Byte(&ptr1, sizeof(temp_colliderData) * colliderSize);

	for (int i = 0; i < colliderSize; i++)
	{
		mColliderDatas[i].position = mTempColliderDatas[i].position;
		mColliderDatas[i].rotation = mTempColliderDatas[i].rotation;
		mColliderDatas[i].scale = mTempColliderDatas[i].scale;
	}

	findCollider("smashAttackCollider", mSmashAttackCollider);
	findCollider("BodyCollider", mBodyCollider);
}

void Mutant::findCollider(string name, Collider* collider)
{
	for (int i = 0; i < mColliderDatas.size(); i++)
	{
		if (mColliderDatas[i].name == name)
		{
			collider->position = mColliderDatas[i].position;
			collider->rotation = mColliderDatas[i].rotation;
			collider->scale = mColliderDatas[i].scale;
		}
	}
}


Collider* Mutant::GetHitCollider()
{
	return mBodyCollider;
}