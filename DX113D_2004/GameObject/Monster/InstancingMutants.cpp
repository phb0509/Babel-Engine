#include "Framework.h"

InstancingMutants::InstancingMutants(int instanceCount, Terrain* terrain) :
	mTerrain(terrain)
{
	//loadBinaryFile(); // ������ ������ �ö��̴� �ҷ�����.

	//mRotation.y = XM_PI;
	//UpdateWorld();

	//mPlayerDetectRange = 15.0f;
	//mDistanceToPlayerForAttack = 7.0f;
	//mCurrentState = GetPatrolState();

	ModelAnimators::SetMesh("Mutant", "Mutant.mesh");
	ModelAnimators::SetMaterial("Mutant", "Mutant.mat");
	//ModelAnimators::SetDiffuseMap()


	//ModelAnimators::SetNormalMap(L"ModelData/Mutant/Mutant_normal.png");

	ModelAnimators::SetShader(L"Models"); // SetShader�ϱ����� Mesh�� Material ���� Set����ߵ�.

	ModelAnimators::ReadClip("Mutant", "Idle.clip");
	ModelAnimators::ReadClip("Mutant", "Run.clip");
	ModelAnimators::ReadClip("Mutant", "SmashAttack.clip");
	ModelAnimators::ReadClip("Mutant", "OnDamage.clip");
	ModelAnimators::ReadClip("Mutant", "Die.clip"); // Mutant�� ���� �����ϰ�.
	ModelAnimators::SetBoxForFrustumCulling();

	for (int i = 0; i < instanceCount; i++)
	{
		Monster* temp = new InstanceMonster();
		temp->SetUpperFrameBuffer(mFrameBuffer);
		mInstanceObjects.push_back(temp);
		ModelAnimators::AddTransform(mInstanceObjects[i]->GetTransform());

	}

	for (int i = 0; i < instanceCount; i++)
	{
		//ModelAnimators::PlayClip(i, static_cast<UINT>(eAnimationStates::Idle));
	}
}

InstancingMutants::~InstancingMutants()
{

}

void InstancingMutants::Update()
{
	for (int i = 0; i < mInstanceObjects.size(); i++)
	{
		mInstanceObjects[i]->Update();
	}

	for (int i = 0; i < mInstanceObjects.size(); i++)
	{
		//mFrameBuffer->data.tweenDesc[i].cur.clip = static_cast<int>(mInstanceObjects[i]->GetAnimationStates());
		/*mFrameBuffer->data.tweenDesc[i].cur.clip = mInstanceObjects[i]->GetCurrentClip();
		mFrameBuffer->data.tweenDesc[i].next.clip = mInstanceObjects[i]->GetNextClip();*/
		//mFrameBuffer->data.tweenDesc[i].next.clip = mInstanceObjects[i]->GetNextClip();
	}

	if (KEY_DOWN('1'))
	{
		SetAnimation(1, eAnimationStates::Run);
	}

	if (KEY_DOWN('2'))
	{
		SetAnimation(1, eAnimationStates::OnDamage);
	}

	if (KEY_DOWN('3'))
	{
		SetAnimation(1, eAnimationStates::Die);
	}

	ModelAnimators::Update();
}

void InstancingMutants::PreRender()
{
}

void InstancingMutants::Render()
{
	ModelAnimators::Render();
}

void InstancingMutants::PostRender()
{
	ImGui::Begin("Instance Information");

	for (int i = 0; i < mInstanceObjects.size(); i++)
	{
		string animationName;

		switch (static_cast<UINT>(mInstanceObjects[i]->GetAnimationStates()))
		{
		case 0:
			animationName = "Idle";
			break;
		case 1:
			animationName = "Run";
			break;
		case 2:
			animationName = "SmashAttack";
			break;
		case 3:
			animationName = "OnDamage";
			break;
		case 4:
			animationName = "Die";
			break;
		}

		string name = to_string(i) + " Mutant State is " + animationName;
		string frameBufferValue = "FrameBufferClipValue : " + to_string(mFrameBuffer->data.tweenDesc[i].cur.clip);
		ImGui::Text(name.c_str());
		ImGui::Text(frameBufferValue.c_str());
		SpacingRepeatedly(2);
	}

	ImGui::End();
}

void InstancingMutants::OnDamage(int instanceIndex, float damage)
{
	//mFSM = eFSMstates::OnDamage;
	//mbOnHit = true;
	//GM->SetHitCheckMap(this, true);
	//mCurrentHP -= 10.0f;
}

void InstancingMutants::CheckOnHit(int instanceIndex)
{
	/*if (!mbOnHit) return;

	SetAnimation(eAnimationStates::OnDamage);*/
}

Collider* InstancingMutants::GetColliderForAStar(int instanceIndex) // ���� �ö��̴� �Ѱ�����.
{
	//return mCollidersMap["bodyCollider"];
	return nullptr;
}

void InstancingMutants::setOnDamageEnd(int instanceIndex)
{
	/*SetAnimation(instanceIndex,eAnimationStates::Idle);
	GM->SetHitCheckMap(this, false);
	mbOnHit = false;*/
}

void InstancingMutants::SetIdle(int instanceIndex)
{
	ModelAnimators::PlayClip(instanceIndex, static_cast<UINT>(eAnimationStates::Idle));
}

void InstancingMutants::SetAnimation(int instanceIndex, eAnimationStates value) // 
{
	if (mInstanceObjects[instanceIndex]->GetAnimationStates() != value)
	{
		mInstanceObjects[instanceIndex]->SetAnimation(value);
		ModelAnimators::PlayClip(instanceIndex,static_cast<UINT>(value));
		//SetAnimation(1, eAnimationStates::Run);
	}
}

void InstancingMutants::setAttackEnd(int instanceIndex)
{
	/*mPlayer = GM->Get()->GetPlayer();
	RotateToDestinationForModel(this, mPlayer->mPosition);*/
}

Collider* InstancingMutants::GetHitCollider(int instanceIndex) // ��Ʈüũ�� �ö��̴�
{
	//return mCollidersMap["bodyCollider"];
	return nullptr;
}

void InstancingMutants::setColliders(int instanceIndex)
{
	//for (int i = 0; i < mColliders.size(); i++)
	//{
	//	string nodeName = mColliders[i].nodeName;
	//	int nodeIndex = GetNodeIndex(nodeName); // �ݺ���������ã�°ǵ� �������̴ϱ� ������̺� ����. �������� �ٷ� ����,������ �׋� get�ϸ�Ǵϱ�.
	//	mColliders[i].matrix = GetTransformByNode(nodeIndex) * this->mWorldMatrix;
	//	mColliders[i].collider->SetParent(&mColliders[i].matrix);
	//}
}

void InstancingMutants::loadBinaryFile()
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