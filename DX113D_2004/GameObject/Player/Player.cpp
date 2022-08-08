#include "Framework.h"

Player::Player(): 
	ModelAnimator(),
	mbIsInitialize(false),
	mAnimationStates(Idle),
	mbIsNormalAttack(false),
	mbIsNormalAttackCollide(false),
	mNormalAttackDamage(1.0f),
	mbIsTargetMode(false),
	mTargetCameraRotationX(0.0f),
	mTargetCameraRotationY(0.0f),
	mbIsLODTerrain(false),
	mbIsCheckNormalAttackCollision(false),
	mMaxHP(100.0f),
	mCurHP(100.0f),
	mHPRate(1.0f),
	mMaxMP(100.0f),
	mCurMP(100.0f),
	mMPRate(1.0f)
{
	SetTag("Player");
	mScale = { 0.05f, 0.05f, 0.05f };
	mMoveSpeed = 50.0f;
	mRotationSpeed = 5.0f;
	mPreFrameMousePosition = MOUSEPOS;

	SetMesh("Player", "Player.mesh");
	SetMaterial("Player", "Player.mat");
	SetShader(L"ModelAnimation");

	ReadClip("Player", "TPose.clip");
	ReadClip("Player", "Idle.clip");
	ReadClip("Player", "Run.clip");
	ReadClip("Player", "Attack.clip");
	ReadClip("Player", "Die.clip");

	SetEndEvent(Run, bind(&Player::setIdle, this));
	SetEndEvent(NormalAttack, bind(&Player::setNormalAttackEnd, this));

	PlayClip(1);

	mRotation.y = XM_PI; // ������� �ݴ�εǾ��־ 180�� ������ߵ�.

	loadBinaryCollidersFile(L"Player.map");
	setAttackInformations();
	UpdateWorld();

	mStatusBar = new PlayerStatusBar();
	mStatusBar->SetTag("PlayerStatusBar");
}

Player::~Player()
{
	for (int i = 0; i < mColliders.size(); i++)
	{
		GM->SafeDelete(mColliders[i].collider);
	}

	GM->SafeDelete(mStatusBar);
}

void Player::Update()
{
	if (!mbIsInitialize)
	{
		initialize();
		mbIsInitialize = true;
	}

	mStatusBar->Update();

	setColliders();
	updateCamera();

	move();
	attack();

	Transform::UpdateWorld();
	ModelAnimator::Update();
}

void Player::Render()
{
	renderColliders();
	Transform::UpdateWorld();
	Transform::SetWorldBuffer();
	ModelAnimator::Render();
}

void Player::DeferredRender()
{
	renderColliders();
	Transform::UpdateWorld();
	Transform::SetWorldBuffer();
	ModelAnimator::DeferredRender();
}

void Player::move()
{
	if (mbIsTargetMode)
	{
		moveInTargetMode();
	}
	else
	{
		moveInWorldMode();
	}
}

void Player::attack()
{
	if (KEY_DOWN(VK_LBUTTON))
	{
		normalAttack();
	}

	checkNormalAttackCollision(); // �⺻���� ���� �浹üũ.
}

void Player::updateCamera()
{
	if (mbIsTargetMode)
	{
		mTargetCamera->Update();
		moveTargetCamera();
	}
	else
	{
		mTargetCameraInWorld->Update();
		moveTargetCameraInWorld();
	}
}

void Player::moveInTargetMode() // Player
{
	if (mbIsNormalAttack) return;

	float terrainY = 0.0f;

	if (mbIsLODTerrain)
	{
		//terrainY = mLODTerrain->GetHeight(mPosition);
	}
	else
	{
		terrainY = mTerrain->GetTargetPositionY(mPosition);
	}
	
	mPosition.y = terrainY;

	if (KEY_PRESS('W'))
	{
		rotateInTargetMode();
		mPosition.z += mTargetCameraForward.z * mMoveSpeed * DELTA * 1.0f;
		mPosition.x += mTargetCameraForward.x * mMoveSpeed * DELTA * 1.0f;

		setAnimation(Run);
	}

	if (KEY_PRESS('S'))
	{
		rotateInTargetMode();
		mPosition.z += mTargetCameraForward.z * -mMoveSpeed * DELTA * 1.0f;
		mPosition.x += mTargetCameraForward.x * -mMoveSpeed * DELTA * 1.0f;

		setAnimation(Run);
	}

	if (KEY_PRESS('A'))
	{
		rotateInTargetMode();
		mPosition += GetRightVector() * mMoveSpeed * DELTA;
	}

	if (KEY_PRESS('D'))
	{
		rotateInTargetMode();
		mPosition += GetRightVector() * -mMoveSpeed * DELTA;
	}
} 

void Player::moveInWorldMode() // Player
{
	if (mbIsNormalAttack) return;

	float terrainY = mTerrain->GetTargetPositionY(mPosition);

	mPosition.y = terrainY;

	if (KEY_PRESS('W'))
	{
		mPosition.z += -GetForwardVector().z * mMoveSpeed * DELTA;
		mPosition.x += -GetForwardVector().x * mMoveSpeed * DELTA;

		setAnimation(Run);
	}

	if (KEY_PRESS('S'))
	{
		mPosition.z -= GetForwardVector().z * -mMoveSpeed * DELTA;
		mPosition.x -= GetForwardVector().x * -mMoveSpeed * DELTA;

		setAnimation(Run);
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

void Player::rotateInTargetMode()
{
	float rotationSpeed = 10.0f * DELTA;

	Vector3 temp = Vector3::Cross(mTargetCameraForward, -1 * GetForwardVector());

	if (temp.y < 0.0f) // �÷��̾� �����庤�Ͱ� ī�޶������庤���� ���ʿ� �ִٸ�. // �� ���������� ȸ���ؾ��Ѵٸ�
	{
		if (CompareFloat(mTargetCameraForward.x, -GetForwardVector().x) &&
			CompareFloat(mTargetCameraForward.z, -GetForwardVector().z))
		{}
		else
		{
			mRotation.y += rotationSpeed;
		}
	}

	else if (temp.y >= 0.0f)// �������� ȸ���ؾ� �Ѵٸ�.
	{
		if (CompareFloat(mTargetCameraForward.x, -GetForwardVector().x) &&
			CompareFloat(mTargetCameraForward.z, -GetForwardVector().z))
		{}
		else
		{
			mRotation.y -= rotationSpeed;
		}
	}
}

// SettedCamera - TargetCamera
void Player::moveTargetCamera()
{
	//m TargetCamera->mRotation = { 0.25f,0.25f,0.0f };
	rotateTargetCamera();

	Matrix targetCameraRotMatrixY = XMMatrixRotationY(mTargetCameraRotationY); // rotY�� FollowControl���� ���콺x��ǥ�� �̵����밪�� ���� ũ������. �� rotY��ŭ �̵��� ����.
	Matrix targetCameraRotMatrixX = XMMatrixRotationX(mTargetCameraRotationX);
	float distance = 13.0f;
	float height = 11.0f;

	Vector3 targetCameraForward = XMVector3TransformNormal(kForward, targetCameraRotMatrixX * targetCameraRotMatrixY); // rotMatrix�� ���⸸? �����µ�.
	//forward = XMVector3TransformNormal(forward, mRotMatrixX);

	Vector3 destPos;
	destPos = targetCameraForward * -distance;
	destPos += GetGlobalPosition();
	destPos.y += height;

	mTargetCamera->mPosition = LERP(mTargetCamera->mPosition, destPos, 5.0f * DELTA); // ī�޶� ��ġ.
																				      // ����ġ���� desPost���� moveDamping����ŭ.

	Vector3 offset = { 0.0f,5.0f,0.0f };
	Vector3 tempOffset = XMVector3TransformCoord(offset.data, targetCameraRotMatrixY);
	Vector3 targetPosition = GetGlobalPosition() + tempOffset;

	//cameraForward = (mPlayer->GlobalPos() - position).Normal();
	mTargetCameraForward = targetCameraForward.Normal();

	Matrix viewMatrix = XMMatrixLookAtLH(mTargetCamera->mPosition.data, targetPosition.data,
		mTargetCamera->GetUpVector().data); // ī�޶���ġ , Ÿ����ġ , ī�޶��� ������

	// �������ҿ� ����� ����.
	mTargetCamera->SetViewMatrix(viewMatrix);
	mTargetCamera->GetViewBuffer()->SetMatrix(viewMatrix);
	mTargetCamera->SetViewToFrustum(viewMatrix);
}

void Player::rotateTargetCamera()
{
	Vector3 value = MOUSEPOS - mPreFrameMousePosition;

	mTargetCameraRotationY += value.x * mRotationSpeed * DELTA;
	mTargetCameraRotationX += value.y * mRotationSpeed * DELTA;

	mPreFrameMousePosition = MOUSEPOS;
}

// SettedCamera - TargetCameraInWorld
void Player::moveTargetCameraInWorld()
{
	Vector3 targetPosition = mPosition;
	Vector3 beforePosition = mTargetCameraInWorld->mPosition;

	mTargetCameraInWorld->mPosition = mPosition + GetForwardVector() * 1.0f; // Ÿ�ٿ�����Ʈ�� ���� ��ġ�� ����.

	Matrix viewMatrix = XMMatrixLookAtLH(mTargetCameraInWorld->mPosition.data, targetPosition.data,
		mTargetCameraInWorld->GetUpVector().data); // ī�޶���ġ , Ÿ����ġ , ī�޶��� ������

	// �������ҿ� ����� ����.
	mTargetCameraInWorld->SetViewMatrix(viewMatrix);
	mTargetCameraInWorld->GetViewBuffer()->SetMatrix(viewMatrix);
	mTargetCameraInWorld->SetViewToFrustum(viewMatrix);
}

void Player::checkNormalAttackCollision()
{
	if (!mbIsNormalAttack) return;

	for (auto monster : mMonsters)
	{
		string monsterName = monster.first;

		for (int i = 0; i < mMonsters[monsterName].size(); i++) // ���� �˻�.
		{
			Monster* monster = mMonsters[monsterName][i].monster;
			bool bWasNormalAttackedBefore = mMonsters[monsterName][i].bIsCheckAttack["NormalAttack"];

			if (!bWasNormalAttackedBefore && !monster->GetIsDie()) // ���� ���ݹ��� ���� ���¸�
			{
				if (monster->CheckIsCollision(mAttackInformations["NormalAttack"].attackColliders[0]))
				{
					monster->OnDamage(mAttackInformations["NormalAttack"]);
					mMonsters[monsterName][i].bIsCheckAttack["NormalAttack"] = true;
				}
			}
		}
	}
}

void Player::setNormalAttackEnd()
{
	setAnimation(Idle);
	mbIsNormalAttack = false;

	for (auto monster : mMonsters)
	{
		string monsterName = monster.first;

		for (int i = 0; i < mMonsters[monsterName].size(); i++) // ���� �˻�.
		{
			mMonsters[monsterName][i].bIsCheckAttack["NormalAttack"] = false;
		}
	}
}

void Player::normalAttack()
{
	if (mbIsNormalAttack) return;
	setAnimation(NormalAttack);
	mbIsNormalAttack = true;
}

void Player::renderColliders()
{
	for (int i = 0; i < mColliders.size(); i++)
	{
		mColliders[i].collider->Render();
	}
}

void Player::setColliders()
{
	for (int i = 0; i < mColliders.size(); i++)
	{
		string nodeName = mColliders[i].nodeName;
		int nodeIndex = mNodeLookupTable[nodeName]; // �ݺ���������ã�°ǵ� �������̴ϱ� ������̺� ����. �������� �ٷ� ����,������ �׋� get�ϸ�Ǵϱ�.
		mColliders[i].matrix = GetTransformByNode(nodeIndex) * this->GetWorldMatrixValue(); // �ش� ����� �����Ʈ����.
		mColliders[i].collider->SetParent(&mColliders[i].matrix); // �ö��̴��� �θ� ����� �����Ʈ������ ����.
		mColliders[i].collider->Update();
	}
}

void Player::setAttackInformations()
{
	// ������ AttackInformation�� �ö��̴� 1���� �ֱ� ������, ���ݿ����� ���� �ö��̴��� �־���� ���� �ֱ���.
	vector<Collider*> temp;

	// NormalAttack
	temp.push_back(mCollidersMap["SwordCollider"]);
	AttackInformation normalAttackInformation("NormalAttack", temp, eAttackType::Normal, mNormalAttackDamage);
	mAttackInformations[normalAttackInformation.attackName] = normalAttackInformation;
	temp.clear();

	mbIsCheckAttack["NormalAttack"] = false;
}

void Player::loadBinaryCollidersFile(wstring fileName)
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

	// Create Colliders;
	for (int i = 0; i < mTempColliderDatas.size(); i++)
	{
		SettedCollider settedCollider;
		Collider* collider = nullptr;

		switch (mTempColliderDatas[i].colliderType)
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
			collider->SetTag(mTempColliderDatas[i].colliderName);
			collider->mPosition = mTempColliderDatas[i].position;
			collider->mRotation = mTempColliderDatas[i].rotation;
			collider->mScale = mTempColliderDatas[i].scale;

			settedCollider.colliderName = mTempColliderDatas[i].colliderName;
			settedCollider.nodeName = mTempColliderDatas[i].nodeName;
			settedCollider.collider = collider;

			mColliders.push_back(settedCollider);
			mCollidersMap[mTempColliderDatas[i].colliderName] = collider;
		}
	}

	binaryReader.CloseReader();
}

void Player::SetMonsters(string name, vector<Monster*> monsters)
{
	for (int i = 0; i < monsters.size(); i++)
	{
		MonsterForAttackCheck temp;
		temp.monster = monsters[i];
		temp.bIsCheckAttack = mbIsCheckAttack; // setAttackInformations()���� �̸� �����س��� ��.
		mMonsters[name].push_back(temp);
	}
}

void Player::initialize()
{
}

void Player::setIdle()
{
	setAnimation(Idle);
}

void Player::setAnimation(eAnimationStates value, bool isForcingPlay)
{
	if (!isForcingPlay)
	{
		if (mAnimationStates != value)
		{
			mAnimationStates = value;
			PlayClip(mAnimationStates);
		}
	}
	else
	{
		mAnimationStates = value;
		PlayClip(mAnimationStates);
	}
}

void Player::PostRender()
{
	ImGui::Begin("Player PostRender");

	ImGui::Text("isNormalAttack : %d\n ", mbIsNormalAttack);

	ImGui::InputFloat3("Player Position", (float*)&this->mPosition);
	SpacingRepeatedly(3);

	for (auto monster : mMonsters)
	{
		string monsterName = monster.first;

		for (int i = 0; i < mMonsters[monsterName].size(); i++) // ���� �˻�.
		{
			Monster* monster = mMonsters[monsterName][i].monster;

			ImGui::Text(monster->GetTag().c_str());
			SpacingRepeatedly(2);
		}
	}

	ImGui::End();

	mStatusBar->PostTransformRender();
	//mStatusBar->PostRender();
}

void Player::UIRender()
{
	mStatusBar->Render();
}
