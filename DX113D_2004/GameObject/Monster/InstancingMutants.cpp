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
		
		string tagName = to_string(i) + " Mutant";
		temp->SetTag(tagName);
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
	showInstanceInformation();
}

void InstancingMutants::OnDamage(int instanceIndex, AttackInformation attackInformation)
{

}

void InstancingMutants::CheckOnHit(int instanceIndex)
{
}

Collider* InstancingMutants::GetColliderForAStar(int instanceIndex) // ���� �ö��̴� �Ѱ�����.
{
	return nullptr;
}

void InstancingMutants::setOnDamageEnd(int instanceIndex)
{
}

void InstancingMutants::SetIdle(int instanceIndex)
{
	ModelAnimators::PlayClip(instanceIndex, static_cast<UINT>(eMutantAnimationStates::Idle));
}

void InstancingMutants::SetAnimation(int instanceIndex, eMutantAnimationStates value) // 
{
	int animationState = static_cast<int>(value);

	if (mInstanceObjects[instanceIndex]->GetAnimationState() != animationState)
	{
		mInstanceObjects[instanceIndex]->SetAnimation(animationState);
		ModelAnimators::PlayClip(instanceIndex, static_cast<UINT>(value));
	}
}

void InstancingMutants::setAttackEnd(int instanceIndex)
{

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
				collider->SetTag(mTempColliderDatas[j].colliderName);
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

void InstancingMutants::showInstanceInformation()
{
	ImGui::Begin("Mutants Information");

	ImGui::Text("RenderingMutants After FrustumCulling : %d", mRenderedInstanceIndices.size()); // �ø� ���� ���� �����Ǵ� �ν��Ͻ�����Ʈ ����.

	SpacingRepeatedly(2);

	for (int i = 0; i < mInstanceObjects.size(); i++)
	{
		string currentFSMStateName;
		string currentAnimationStateName;

		switch (static_cast<UINT>(mInstanceObjects[i]->GetEnumFSMState()))
		{
		case 0:
			currentFSMStateName = "Patrol";
			break;
		case 1:
			currentFSMStateName = "Stalk";
			break;
		case 2:
			currentFSMStateName = "Attack";
			break;
		case 3:
			currentFSMStateName = "OnDamage";
			break;
		case 4:
			currentFSMStateName = "Die";
			break;
		}

		switch (static_cast<UINT>(mInstanceObjects[i]->GetAnimationState())) 
		{
		case 0:
			currentAnimationStateName = "Idle";
			break;
		case 1:
			currentAnimationStateName = "Run";
			break;
		case 2:
			currentAnimationStateName = "SmashAttack";
			break;
		case 3:
			currentAnimationStateName = "OnDamage";
			break;
		case 4:
			currentAnimationStateName = "Die";
			break;
		}

		string fsmStateStringToPrint = to_string(i) + " Mutant FSMState       : " + currentFSMStateName;
		string animaionStateStringToPrint = to_string(i) + " Mutant AnimationState : " + currentAnimationStateName;
		string frameBufferValue = "FrameBufferClipValue : " + to_string(mFrameBuffer->data.tweenDesc[i].cur.clip);
		string bIsCurrentAnimationEnd = "";

		if (mInstanceObjects[i]->GetCurrentAnimationEnd())
		{
			bIsCurrentAnimationEnd = "Animation is End";
		}
		else
		{
			bIsCurrentAnimationEnd = "Animation is Playing...";
		}
	
		ImGui::Separator();

		ImGui::Text(fsmStateStringToPrint.c_str());
		ImGui::Text(animaionStateStringToPrint.c_str());
		//ImGui::Text(bIsCurrentAnimationEnd.c_str());
		ImGui::Text(frameBufferValue.c_str());

		ImGui::Separator();
		SpacingRepeatedly(3);
	}

	ImGui::End();
}
