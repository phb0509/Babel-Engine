#include "Framework.h"

Player::Player()
	: ModelAnimator(), 
	mbIsInitialize(false), 
	state(IDLE), 
	mbIsNormalAttack(false),
	mbIsNormalAttackCollide(false), 
	mNormalAttackDamage(10.0f)
{
	mScale = { 0.05f, 0.05f, 0.05f };
	mMoveSpeed = 50.0f;
	mRotationSpeed = 5.0f;

	SetMesh("Player", "Player.mesh");
	SetMaterial("Player", "Player.mat");

	SetShader(L"ModelAnimation");

	ReadClip("Player","TPose.clip");
	ReadClip("Player","Idle.clip");
	ReadClip("Player","Run.clip");
	ReadClip("Player","Attack.clip");
	ReadClip("Player","Die.clip");

	SetEndEvent(RUN, bind(&Player::setIdle, this));
	SetEndEvent(ATTACK, bind(&Player::setAttackEnd, this));

	PlayClip(1);

	mRotation.y = XM_PI; // 포워드랑 반대로되어있어서 180도 돌려줘야됨.

	loadBinaryFile();
	UpdateWorld();
}

Player::~Player()
{

}

void Player::Update()
{
	if (!mbIsInitialize)
	{
		initialize();
		mbIsInitialize = true;
	}

	setColliders();
	
	for (int i = 0; i < mColliders.size(); i++)
	{
		mColliders[i].collider->Update();
	}

	input();

	checkNormalAttackCollision(); // 기본공격 몬스터 충돌체크.

	UpdateWorld();
	ModelAnimator::Update();
}

void Player::Render()
{
	for (int i = 0; i < mColliders.size(); i++)
	{
		mColliders[i].collider->Render();
	}

	SetWorldBuffer();
	ModelAnimator::Render();
}



void Player::input()
{
	if (Environment::Get()->GetIsTargetCamera())
	{
		moveInTargetCamera();
	}
	else
	{
		moveInWorldCamera();
	}
	

	if (KEY_DOWN(VK_LBUTTON))
	{
		normalAttack();
	}
}

void Player::moveInTargetCamera()
{
	if (mbIsNormalAttack) return;

	float terrainY = mTerrain->GetHeight(mPosition);

	mPosition.y = terrainY;
	
	if (KEY_PRESS('W'))
	{
		rotate();

		mPosition.z += TARGETCAMERA->mCameraForward.z * mMoveSpeed * DELTA * 1.0f;
		mPosition.x += TARGETCAMERA->mCameraForward.x * mMoveSpeed * DELTA * 1.0f;

		setAnimation(RUN);
	}

	if (KEY_PRESS('S'))
	{
		rotate();
		mPosition.z += TARGETCAMERA->mCameraForward.z * -mMoveSpeed * DELTA * 1.0f;
		mPosition.x += TARGETCAMERA->mCameraForward.x * -mMoveSpeed * DELTA * 1.0f;

		setAnimation(RUN);
	}

	if (KEY_PRESS('A'))
	{
		rotate();
		mPosition += Right() * mMoveSpeed * DELTA;
	}

	if (KEY_PRESS('D'))
	{
		rotate();
		mPosition += Right() * -mMoveSpeed * DELTA;
	}
}

void Player::moveInWorldCamera()
{
	if (mbIsNormalAttack) return;

	float terrainY = mTerrain->GetHeight(mPosition);

	mPosition.y = terrainY;

	if (KEY_PRESS('W'))
	{
		mPosition.z += -Forward().z * mMoveSpeed * DELTA;
		mPosition.x += -Forward().x * mMoveSpeed * DELTA;

		setAnimation(RUN);
	}

	if (KEY_PRESS('S'))
	{
		mPosition.z -= Forward().z * -mMoveSpeed * DELTA;
		mPosition.x -= Forward().x * -mMoveSpeed * DELTA;

		setAnimation(RUN);
	}

	if (KEY_PRESS('A'))
	{
		mRotation.y -= mRotationSpeed * DELTA;
	}

	if (KEY_PRESS('D'))
	{
		mRotation.y += mRotationSpeed * DELTA;
	}
}

void Player::rotate()
{
	Vector3 temp = Vector3::Cross(TARGETCAMERA->mCameraForward, -1 * Forward());

	if (temp.y < 0.0f) // 플레이어 포워드벡터가 카메라포워드벡터의 왼쪽에 있다면. // 즉 오른쪽으로 회전해야한다면
	{
		if (CompareFloat(TARGETCAMERA->mCameraForward.x, -Forward().x) &&
			CompareFloat(TARGETCAMERA->mCameraForward.z, -Forward().z))
		{	}
		else
		{
			mRotation.y += 0.02f;
		}
	}

	else if(temp.y >= 0.0f)// 왼쪽으로 회전해야 한다면.
	{
		if (CompareFloat(TARGETCAMERA->mCameraForward.x, -Forward().x) &&
			CompareFloat(TARGETCAMERA->mCameraForward.z, -Forward().z))
		{	}
		else
		{
			mRotation.y -= 0.02f;
		}
	}
}

void Player::checkNormalAttackCollision()
{
	if (mbIsNormalAttack) // 공격도중이면.
	{
		mMonsters = GM->GetMonsters();

		int a = 0;
		for (int i = 0; i < mMonsters.size(); i++)
		{
			if (!(GM->GetHitCheckMap()[mMonsters[i]])) // 공격받을수있는 상황이면(한프레임도 아직 공격받지 않았다면
			{
				if (mCollidersMap["swordCollider"]->Collision(mMonsters[i]->GetHitCollider())) // 고놈만 충돌ㅇ체크.
				{
					int a = 0;
					mMonsters[i]->OnDamage(mNormalAttackDamage);
				}
			}
		}
	}
	
}





void Player::setAttackEnd()
{
	setAnimation(IDLE);
	mbIsNormalAttack = false;

}

void Player::normalAttack()
{
	if (mbIsNormalAttack) return;
	setAnimation(ATTACK);
	mbIsNormalAttack = true;
}

void Player::setColliders()
{
	for (int i = 0; i < mColliders.size(); i++)
	{
		string nodeName = mColliders[i].nodeName;
		int nodeIndex = GetNodeIndex(nodeName); // 반복문돌려서찾는건데 고정값이니까 룩업테이블 따로. 값있으면 바로 쓰고,없으면 그떄 get하면되니까.
		mColliders[i].matrix = GetTransformByNode(nodeIndex) * this->mWorldMatrix;
		mColliders[i].collider->SetParent(&mColliders[i].matrix);
	}
}

void Player::loadBinaryFile()
{
	BinaryReader binaryReader(L"TextData/Player.map");
	UINT colliderCount = binaryReader.UInt();

	mColliderSRTdatas.resize(colliderCount);
	mColliderDatas.resize(colliderCount);

	void* ptr1 = (void*)mColliderSRTdatas.data();

	for (int i = 0; i < colliderCount; i++)
	{
		mColliderDatas[i].colliderName = binaryReader.String();
		mColliderDatas[i].nodeName = binaryReader.String();
	}

	binaryReader.Byte(&ptr1, sizeof(TempCollider) * colliderCount);

	for (int i = 0; i < colliderCount; i++)
	{
		mColliderDatas[i].position = mColliderSRTdatas[i].position;
		mColliderDatas[i].rotation = mColliderSRTdatas[i].rotation;
		mColliderDatas[i].scale = mColliderSRTdatas[i].scale;
	}

	// Create Colliders;
	for (int i = 0; i < mColliderDatas.size(); i++)
	{
		SettedCollider settedCollider;
		Collider* collider = new BoxCollider(); // 일단 박스

		collider->mTag = mColliderDatas[i].colliderName;
		collider->mPosition = mColliderDatas[i].position;
		collider->mRotation = mColliderDatas[i].rotation;
		collider->mScale = mColliderDatas[i].scale;

		settedCollider.colliderName = mColliderDatas[i].colliderName;
		settedCollider.nodeName = mColliderDatas[i].nodeName;
		settedCollider.collider = collider;

		mColliders.push_back(settedCollider);
		mCollidersMap[mColliderDatas[i].colliderName] = collider;
	}

	binaryReader.CloseReader();
}

void Player::initialize()
{
	
}

void Player::setIdle()
{
	setAnimation(IDLE);
}

void Player::setAnimation(State value)
{
	if (state != value)
	{
		state = value;
		PlayClip(state);
	}
}

void Player::PostRender()
{
	ImGui::Begin("Test");

	ImGui::Text("isNormalAttack : %d\n ", mbIsNormalAttack);

	for (int i = 0; i < GM->GetMonsters().size(); i++)
	{
		ImGui::Text("%d Monster is Attacked?  : %d ", i, GM->GetHitCheckMap()[GM->GetMonsters()[i]]);
	}

	ImGui::InputFloat3("Player Position", (float*)&this->mPosition);
	Spacing(1);
	ImGui::InputFloat3("Mutant Position", (float*)&GM->GetMonsters()[0]->mPosition);
	Spacing(1);
	ImGui::InputFloat3("PlayerSword Position", (float*)&mCollidersMap["swordCollider"]->mPosition);
	Spacing(1);
	ImGui::InputFloat3("MutantBody Position", (float*)&GM->GetMonsters()[0]->GetHitCollider()->mPosition);

	//ImGui::InputFloat3("SwordRotation", (float*)&GM->GetMonsters()[i]->mRotation);
	//ImGui::InputFloat3("SwordScale", (float*)&GM->GetMonsters()[i]->mScale);

	ImGui::End();
}