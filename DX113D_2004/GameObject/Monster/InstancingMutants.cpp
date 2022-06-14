#include "Framework.h"

InstancingMutants::InstancingMutants(int instanceCount, Terrain* terrain) :
	mInstanceCount(instanceCount),
	mTerrain(terrain)
{
	loadBinaryCollidersFile(L"Mutant.map"); // ������ ������ �ö��̴� �ҷ�����.

	ModelAnimators::SetMesh("Mutant", "Mutant.mesh");
	ModelAnimators::SetMaterial("Mutant", "Mutant.mat");

	ModelAnimators::SetShader(L"Models"); // SetShader�ϱ����� Mesh�� Material ���� Set����ߵ�.

	ModelAnimators::ReadClip("Mutant", "Idle.clip");
	ModelAnimators::ReadClip("Mutant", "Run.clip");
	ModelAnimators::ReadClip("Mutant", "SmashAttack.clip");
	ModelAnimators::ReadClip("Mutant", "OnDamage.clip");
	ModelAnimators::ReadClip("Mutant", "Die.clip"); // Mutant�� ���� �����ϰ�.
	ModelAnimators::SetBoxForFrustumCulling();

	for (int i = 0; i < instanceCount; i++)
	{
		Monster* temp = new InstanceMutant();
		temp->SetInstanceCollider(mInstanceColliderDatas[i]);
		temp->SetUpperFrameBuffer(mFrameBuffer);
		mInstanceObjects.push_back(temp);
		ModelAnimators::AddTransform(mInstanceObjects[i]->GetTransform());
	}
}

InstancingMutants::~InstancingMutants()
{
	for (int i = 0; i < mInstanceObjects.size(); i++)
	{
		delete mInstanceObjects[i];
		mInstanceObjects[i] = nullptr;
	}
}

void InstancingMutants::Update()
{
	for (int i = 0; i < mInstanceObjects.size(); i++)
	{
		mInstanceObjects[i]->Update();
	}

	ModelAnimators::Update(); // Animation Tweening and Frustum Culling
	setColliders();
}

void InstancingMutants::PreRender()
{
}

void InstancingMutants::Render()
{
	ModelAnimators::Render();
	renderColliders();
}

void InstancingMutants::PostRender()
{
	showAnimationStates();

	/*ImGui::Begin("TestWindow");
	ImGui::InputFloat3("Instance0 Position", (float*)&mInstanceObjects[0]->mPosition);
	SpacingRepeatedly(2);
	ImGui::InputFloat3("Body Collider Position", (float*)&mInstanceColliderDatas[0].mColliders[0].collider->mPosition);
	ImGui::End();*/
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
		ModelAnimators::PlayClip(instanceIndex, static_cast<UINT>(value));
		//SetAnimation(1, eAnimationStates::Run);
	}
}

void InstancingMutants::setAttackEnd(int instanceIndex)
{
	/*mPlayer = GM->Get()->GetPlayer();
	RotateToDestinationForModel(this, mPlayer->mPosition);*/
}

void InstancingMutants::renderColliders()
{
	for (int i = 0; i < mRenderedInstanceIndices.size(); i++) // ���� �����Ǵ� �ν��Ͻ��� �ε���
	{
		int renderedInstanceIndex = mRenderedInstanceIndices[i];

		for (int j = 0; j < mInstanceColliderDatas[renderedInstanceIndex].colliders.size(); j++)
		{
			mInstanceColliderDatas[renderedInstanceIndex].colliders[j].collider->Render();
		}
	}
}

Collider* InstancingMutants::GetHitCollider(int instanceIndex) // ��Ʈüũ�� �ö��̴�
{
	//return mCollidersMap["bodyCollider"];
	return nullptr;
}

void InstancingMutants::setColliders()
{
	// Ʈ������ ������Ʈ�� �Ⱥ��̴��� ������Ʈ.

	for (int i = 0; i < mInstanceCount; i++) // ���� �����Ǵ� �ν��Ͻ��� �ε���
	{
		for (int j = 0; j < mInstanceColliderDatas[i].colliders.size(); j++) // ����Ʈ�� ���õ� �ö��̴�����.
		{
			string nodeName = mInstanceColliderDatas[i].colliders[j].nodeName;
			int nodeIndex = GetNodeIndex(nodeName); // �ݺ���������ã�°ǵ� �������̴ϱ� ������̺� ����. �������� �ٷ� ����,������ �׋� get�ϸ�Ǵϱ�.
			mInstanceColliderDatas[i].colliders[j].matrix = GetTransformByNode(i,nodeIndex) * mInstanceObjects[i]->GetWorldMatrixValue(); // ����� �������. ex) LeftHand
			mInstanceColliderDatas[i].colliders[j].collider->SetParent(&mInstanceColliderDatas[i].colliders[j].matrix);
			mInstanceColliderDatas[i].colliders[j].collider->Update();
		}
	}
}

void InstancingMutants::loadBinaryCollidersFile(wstring fileName)
{
	wstring temp = L"TextData/" + fileName;
	//BinaryReader binaryReader(L"TextData/Mutant.map");
	BinaryReader binaryReader(temp);
	UINT colliderCount = binaryReader.UInt();
	int colliderType;

	mTempColliderSRTdatas.resize(colliderCount);
	mTempColliderDatas.resize(colliderCount);

	void* ptr1 = (void*)mTempColliderSRTdatas.data();

	for (int i = 0; i < colliderCount; i++)
	{
		mTempColliderDatas[i].colliderName = binaryReader.String();
		mTempColliderDatas[i].nodeName = binaryReader.String();
		mTempColliderDatas[i].colliderType = binaryReader.UInt();
	}

	binaryReader.Byte(&ptr1, sizeof(TempCollider) * colliderCount);

	for (int i = 0; i < colliderCount; i++)
	{
		mTempColliderDatas[i].position = mTempColliderSRTdatas[i].position;
		mTempColliderDatas[i].rotation = mTempColliderSRTdatas[i].rotation;
		mTempColliderDatas[i].scale = mTempColliderSRTdatas[i].scale;
	}

	mInstanceColliderDatas.assign(mInstanceCount, InstanceColliderData()); // �ν��Ͻ�������ŭ �ʱ�ȭ.

	// Create Colliders;
	for (int i = 0; i < mInstanceCount; i++)
	{
		vector<SettedCollider> tempSettedCollider;
		map<string, Collider*> tempCollidersMap;

		for (int j = 0; j < mTempColliderDatas.size(); j++)
		{
			SettedCollider settedCollider;
			Collider* collider = nullptr;

			switch (mTempColliderDatas[j].colliderType)
			{
			case 0: collider = new BoxCollider();
				break;
			case 1: collider = new SphereCollider();
				break;
			case 2: collider = new CapsuleCollider();
				break;
			default:
				break;
			}

			if (collider != nullptr)
			{
				collider->mTag = mTempColliderDatas[j].colliderName;
				collider->mPosition = mTempColliderDatas[j].position;
				collider->mRotation = mTempColliderDatas[j].rotation;
				collider->mScale = mTempColliderDatas[j].scale;

				settedCollider.colliderName = mTempColliderDatas[j].colliderName;
				settedCollider.nodeName = mTempColliderDatas[j].nodeName;
				settedCollider.collider = collider;

				mInstanceColliderDatas[i].colliders.push_back(settedCollider);
				mInstanceColliderDatas[i].collidersMap[mTempColliderDatas[j].colliderName] = collider;
			}
		}
	}

	binaryReader.CloseReader();
}

void InstancingMutants::showAnimationStates()
{
	ImGui::Begin("Mutants Information");

	ImGui::Text("RenderingMutants After FrustumCulling : %d", mRenderedInstanceIndices.size()); // �ø� ���� ���� �����Ǵ� �ν��Ͻ�����Ʈ ����.

	SpacingRepeatedly(2);

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
