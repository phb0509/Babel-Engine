#include "Framework.h"

InstanceMutant::InstanceMutant(int instanceCount)
{
	//loadBinaryFile(); // 툴에서 셋팅한 컬라이더 불러오기.

	//mRotation.y = XM_PI;
	//UpdateWorld();

	//mPlayerDetectRange = 15.0f;
	//mDistanceToPlayerForAttack = 7.0f;
	//mCurrentState = GetPatrolState();

	ModelAnimators::SetMesh("Mutant", "Mutant.mesh");
	ModelAnimators::SetMaterial("Mutant", "Mutant.mat");

	ModelAnimators::SetShader(L"Models"); // SetShader하기전에 Mesh랑 Material 먼저 Set해줘야됨.

	ModelAnimators::ReadClip("Mutant", "Idle.clip");
	ModelAnimators::ReadClip("Mutant", "Run.clip");
	ModelAnimators::ReadClip("Mutant", "SmashAttack.clip");
	ModelAnimators::ReadClip("Mutant", "OnDamage.clip");
	ModelAnimators::ReadClip("Mutant", "Die.clip");

	ModelAnimators::SetBoxForFrustumCulling();

	for (int i = 0; i < instanceCount; i++)
	{
		Monster* temp = new InstanceMonster();
		mInstanceObjects.push_back(temp);
		ModelAnimators::AddTransform(mInstanceObjects[i]->GetTransform());

		/*ModelAnimators::SetEndEvents(i, 1,
			bind(&InstanceMutant::SetIdle, this, placeholders::_1));*/
	}


	for (int i = 0; i < instanceCount; i++)
	{
		//mInstanceObjects[i]->PlayClip(i, eAnimationStates::Idle);
		ModelAnimators::PlayClip(i, static_cast<UINT>(eAnimationStates::Idle));
	}

	//mModelAnimators->SetDrawCount(instanceCount);

	//ModelAnimator::SetEndEvent(static_cast<int>(eAnimationStates::Run), bind(&Mutant::SetIdle, this));
	//ModelAnimator::SetEndEvent(static_cast<int>(eAnimationStates::OnDamage), bind(&Mutant::setOnDamageEnd, this));
	//ModelAnimator::SetEndEvent(static_cast<int>(eAnimationStates::SmashAttack), bind(&Mutant::setAttackEnd, this));
}

InstanceMutant::~InstanceMutant()
{
}

void InstanceMutant::Update()
{
	for (int i = 0; i < mInstanceObjects.size(); i++)
	{
		//mInstanceOjbects[i]->mCurrentState->Execute(this);
	}

	ModelAnimators::Update();
}

void InstanceMutant::PreRender()
{
}

void InstanceMutant::Render()
{
	ModelAnimators::Render();
}

void InstanceMutant::PostRender()
{
}

void InstanceMutant::OnDamage(int instanceIndex, float damage)
{
	//mFSM = eFSMstates::OnDamage;
	//mbOnHit = true;
	//GM->SetHitCheckMap(this, true);
	//mCurrentHP -= 10.0f;
}

void InstanceMutant::CheckOnHit(int instanceIndex)
{
	/*if (!mbOnHit) return;

	SetAnimation(eAnimationStates::OnDamage);*/
}

Collider* InstanceMutant::GetColliderForAStar(int instanceIndex) // 몸쪽 컬라이더 넘겨주자.
{
	//return mCollidersMap["bodyCollider"];
	return nullptr;
}

void InstanceMutant::setOnDamageEnd(int instanceIndex)
{
	/*SetAnimation(instanceIndex,eAnimationStates::Idle);
	GM->SetHitCheckMap(this, false);
	mbOnHit = false;*/
}


void InstanceMutant::SetIdle(int instanceIndex)
{
	ModelAnimators::PlayClip(instanceIndex, 0);
}

void InstanceMutant::SetAnimation(int instanceIndex, eAnimationStates value)
{
	if (mInstanceObjects[instanceIndex]->GetAnimationStates() != value)
	{
		mInstanceObjects[instanceIndex]->SetAnimation(value);
		ModelAnimators::PlayClip(instanceIndex,static_cast<int>(value));
	}
}

void InstanceMutant::setAttackEnd(int instanceIndex)
{
	/*mPlayer = GM->Get()->GetPlayer();
	RotateToDestinationForModel(this, mPlayer->mPosition);*/
}

Collider* InstanceMutant::GetHitCollider(int instanceIndex) // 히트체크용 컬라이더
{
	//return mCollidersMap["bodyCollider"];
	return nullptr;
}

void InstanceMutant::setColliders(int instanceIndex)
{
	//for (int i = 0; i < mColliders.size(); i++)
	//{
	//	string nodeName = mColliders[i].nodeName;
	//	int nodeIndex = GetNodeIndex(nodeName); // 반복문돌려서찾는건데 고정값이니까 룩업테이블 따로. 값있으면 바로 쓰고,없으면 그떄 get하면되니까.
	//	mColliders[i].matrix = GetTransformByNode(nodeIndex) * this->mWorldMatrix;
	//	mColliders[i].collider->SetParent(&mColliders[i].matrix);
	//}
}

void InstanceMutant::loadBinaryFile()
{
	//BinaryReader binaryReader(L"TextData/Mutant.map");
	//UINT colliderCount = binaryReader.UInt();
	//int colliderType;

	//mColliderSRTdatas.resize(colliderCount);
	//mColliderDatas.resize(colliderCount);

	//void* ptr1 = (void*)mColliderSRTdatas.data();

	//for (int i = 0; i < colliderCount; i++)
	//{
	//	mColliderDatas[i].colliderName = binaryReader.String();
	//	mColliderDatas[i].nodeName = binaryReader.String();
	//	mColliderDatas[i].colliderType = binaryReader.UInt();
	//}

	//binaryReader.Byte(&ptr1, sizeof(TempCollider) * colliderCount);

	//for (int i = 0; i < colliderCount; i++)
	//{
	//	mColliderDatas[i].position = mColliderSRTdatas[i].position;
	//	mColliderDatas[i].rotation = mColliderSRTdatas[i].rotation;
	//	mColliderDatas[i].scale = mColliderSRTdatas[i].scale;
	//}

	//// Create Colliders;
	//for (int i = 0; i < mColliderDatas.size(); i++)
	//{
	//	SettedCollider settedCollider;
	//	Collider* collider = nullptr;

	//	switch (mColliderDatas[i].colliderType)
	//	{
	//	case 0: collider = new BoxCollider();
	//		break;
	//	case 1: collider = new SphereCollider();
	//		break;
	//	case 2: collider = new CapsuleCollider();
	//		break;
	//	default:
	//		break;
	//	}

	//	if (collider != nullptr)
	//	{
	//		collider->mTag = mColliderDatas[i].colliderName;
	//		collider->mPosition = mColliderDatas[i].position;
	//		collider->mRotation = mColliderDatas[i].rotation;
	//		collider->mScale = mColliderDatas[i].scale;

	//		settedCollider.colliderName = mColliderDatas[i].colliderName;
	//		settedCollider.nodeName = mColliderDatas[i].nodeName;
	//		settedCollider.collider = collider;

	//		mColliders.push_back(settedCollider);
	//		mCollidersMap[mColliderDatas[i].colliderName] = collider;
	//	}
	//}

	//binaryReader.CloseReader();
}