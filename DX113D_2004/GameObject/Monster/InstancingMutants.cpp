#include "Framework.h"


bool compare(Monster* a, Monster* b)
{
	Vector3 targetPosition = GM->GetPlayer()->mPosition;

	float aLength = (a->mPosition - targetPosition).Length();
	float bLength = (b->mPosition - targetPosition).Length();

	return aLength > bLength;
}

InstancingMutants::InstancingMutants(int instanceCount, Terrain* terrain) :
	ModelAnimators(instanceCount),
	mInstanceCount(instanceCount),
	mTerrain(terrain)
{
	loadBinaryCollidersFile(L"Mutant.map"); // 툴에서 셋팅한 컬라이더 불러오기.

	ModelAnimators::SetMesh("Mutant", "Mutant.mesh");
	ModelAnimators::SetMaterial("Mutant", "Mutant.mat");

	ModelAnimators::SetShader(L"Models"); // SetShader하기전에 Mesh랑 Material 먼저 Set해줘야됨.

	ModelAnimators::ReadClip("Mutant", "Idle.clip");
	ModelAnimators::ReadClip("Mutant", "Run.clip");
	ModelAnimators::ReadClip("Mutant", "SmashAttack.clip");
	ModelAnimators::ReadClip("Mutant", "OnDamage.clip");
	ModelAnimators::ReadClip("Mutant", "Die.clip"); // Mutant와 순서 동일하게.
	ModelAnimators::SetBoxForFrustumCulling();

	mUIOffset = { -170.0f, 0.0f, 0.0f };

	for (int i = 0; i < instanceCount; i++)
	{
		Monster* monster = new InstanceMutant();
		monster->SetInstanceCollider(mInstanceColliderDatas[i]);
		monster->SetUpperFrameBuffer(mFrameBuffer);
		string tagName = to_string(i) + " Mutant";
		monster->SetTag(tagName);
		mInstanceObjects.push_back(monster);
		ModelAnimators::AddTransform(monster->GetTransform());
		monster->SetPortraitTexture(Texture::Add(L"UI_Resource/Monster_Status/Mutant_Portrait.png"));
	}
}

InstancingMutants::~InstancingMutants()
{
	GM->SafeDeleteVector(mInstanceObjects);
}

void InstancingMutants::Update()
{
	for (int i = 0; i < mInstanceObjects.size(); i++)
	{
		if (!mInstanceObjects[i]->GetIsActive()) continue;
		mInstanceObjects[i]->Update();
		mInstanceObjects[i]->SetUIOffset(mUIOffset);
	}

	updateCompletedAnimIndices(false);
	updateStartedAnimIndices(false);

	ModelAnimators::Update(); // Animation Tweening and Frustum Culling       

	updateCompletedAnimIndices(true);
	updateStartedAnimIndices(true);

	setColliders();
}

void InstancingMutants::PreRender()
{
}

void InstancingMutants::Render()
{
	ModelAnimators::Render();
	renderColliders();
	RenderDebugMode();
}

void InstancingMutants::PostRender()
{
	showInstanceInformation();
	//ImGui::InputFloat3("Global Translation", (float*)&mUIOffset);
}

void InstancingMutants::UIRender()
{
	mZsortedObjects.clear();

	for (int i = 0; i < mRenderedInstanceIndices.size(); i++) // 실제 렌더되는 인스턴스들 인덱스
	{
		int renderedInstanceIndex = mRenderedInstanceIndices[i];
		mZsortedObjects.push_back(mInstanceObjects[renderedInstanceIndex]); // 실제렌더되는 오브젝트들 모아놓기.
	}     

	sort(mZsortedObjects.begin(), mZsortedObjects.end(), compare);
	
	for (int i = 0; i < mZsortedObjects.size(); i++)
	{
		mZsortedObjects[i]->UIRender();
	}
}

void InstancingMutants::RenderDebugMode()
{
	if (DM->GetIsDebugMode())
	{
		/*for (Monster* monster : mInstanceObjects)
		{
			Vector3 screenPos = WorldToScreen(monster->GetGlobalPosition(),mCurMainCamera);

			POINT size = { 200, 100 };
			RECT rect;
			rect.left = screenPos.x;
			rect.top = screenPos.y;
			rect.right = rect.left + size.x;
			rect.bottom = rect.top + size.y;

			DirectWrite::Get()->RenderText(ToWString(monster->GetTag()), rect);
			int a = 0;
		}*/
	}
}

void InstancingMutants::SetIdle(int instanceIndex)
{
	ModelAnimators::PlayClip(instanceIndex, static_cast<UINT>(eMutantAnimationStates::Idle));
}

void InstancingMutants::SetAnimation(int instanceIndex, eMutantAnimationStates value) // 안쓰임.
{
	int animationState = static_cast<int>(value);

	if (mInstanceObjects[instanceIndex]->GetAnimationState() != animationState)
	{
		mInstanceObjects[instanceIndex]->SetAnimation(animationState);
		ModelAnimators::PlayClip(instanceIndex, static_cast<UINT>(value));
	}
}

void InstancingMutants::SetCurMainCamera(Camera* mainCamera)
{
	mCurMainCamera = mainCamera; 
	
	for (Monster* monster : mInstanceObjects)
	{
		monster->SetCurMainCamera(mainCamera);
	}
}

void InstancingMutants::renderColliders()
{
	for (int i = 0; i < mRenderedInstanceIndices.size(); i++) // 실제 렌더되는 인스턴스들 인덱스
	{
		int renderedInstanceIndex = mRenderedInstanceIndices[i];

		for (int j = 0; j < mInstanceColliderDatas[renderedInstanceIndex].colliders.size(); j++)
		{
			mInstanceColliderDatas[renderedInstanceIndex].colliders[j].collider->Render();
		}
	}
}

void InstancingMutants::setColliders()
{
	// 트랜스폼 업데이트는 안보이더라도 업데이트.

	for (int i = 0; i < mInstanceCount; i++) // 실제 렌더되는 인스턴스들 인덱스
	{
		for (int j = 0; j < mInstanceColliderDatas[i].colliders.size(); j++) // 뮤턴트에 셋팅된 컬라이더개수.
		{
			string nodeName = mInstanceColliderDatas[i].colliders[j].nodeName;
			int nodeIndex = mNodeLookupTable[nodeName]; // 반복문돌려서찾는건데 고정값이니까 룩업테이블 따로. 값있으면 바로 쓰고,없으면 그떄 get하면되니까.
			mInstanceColliderDatas[i].colliders[j].matrix = GetTransformByNode(i,nodeIndex) * mInstanceObjects[i]->GetWorldMatrixValue(); // 노드의 월드행렬. ex) LeftHand
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

	mInstanceColliderDatas.assign(mInstanceCount, InstanceColliderData()); // 인스턴스개수만큼 초기화.

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

	ImGui::Text("RenderingMutants After FrustumCulling : %d", mRenderedInstanceIndices.size()); // 컬링 이후 실제 렌더되는 인스턴스뮤턴트 개수.

	SpacingRepeatedly(2);

	for (int i = 0; i < mInstanceObjects.size(); i++)
	{
		Monster* monster = mInstanceObjects[i];
		string currentFSMStateName;
		string currentAnimationStateName;

		switch (static_cast<UINT>(monster->GetEnumFSMState()))
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

		switch (static_cast<UINT>(monster->GetAnimationState()))
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

		if (monster->GetCurrentAnimationEnd())
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

		string currentHP = to_string((int)monster->GetCurHP()) + " / " + to_string((int)monster->GetMaxHP());
		ImGui::Text(currentHP.c_str());

		string tagPosition = monster->GetTag() + " Position";
		string tagRotation = monster->GetTag() + " Rotation";
		string tagScale = monster->GetTag() + " Scale";

		ImGui::InputFloat3(tagPosition.c_str(), (float*)&monster->mPosition);
		SpacingRepeatedly(1);
		ImGui::InputFloat3(tagRotation.c_str(), (float*)&monster->mRotation);
		SpacingRepeatedly(1);
		ImGui::InputFloat3(tagScale.c_str(), (float*)&monster->mScale);
		SpacingRepeatedly(1);
		//monster->PostTransformRender();

		ImGui::Separator();

		SpacingRepeatedly(3);
	}

	ImGui::End();
}

void InstancingMutants::updateCompletedAnimIndices(bool value)
{
	for (int i = 0; i < mCompletedAnimInstanceIndices.size(); i++)
	{
		mInstanceObjects[mCompletedAnimInstanceIndices[i]]->SetIsCompletedAnim(value);
	}
}

void InstancingMutants::updateStartedAnimIndices(bool value)
{
	for (int i = 0; i < mStartedAnimInstanceIndices.size(); i++)
	{
		mInstanceObjects[mStartedAnimInstanceIndices[i]]->SetIsStartedAnim(value);
	}
}

