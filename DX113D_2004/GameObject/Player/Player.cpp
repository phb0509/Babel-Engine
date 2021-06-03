#include "Framework.h"

Player::Player()
	: ModelAnimator("Player/Player"), 
	isInitialize(false), 
	state(IDLE), 
	mIsNormalAttack(false),
	mIsNormalAttackCollide(false), 
	normalAttackDamage(10.0f)
{
	mScale = { 0.05f, 0.05f, 0.05f };
	mMoveSpeed = 50.0f;
	mRotationSpeed = 5.0f;

	SetShader(L"ModelAnimation");

	ReadClip("Player/TPose0.clip");
	ReadClip("Player/Idle0.clip");
	ReadClip("Player/Run0.clip");
	ReadClip("Player/Attack0.clip");
	ReadClip("Player/Die0.clip");

	SetEndEvent(RUN, bind(&Player::setIdle, this));
	SetEndEvent(ATTACK, bind(&Player::setAttackEnd, this));


	PlayClip(1);

	mRotation.y = XM_PI;

	bodyCollider = new BoxCollider();
	swordCollider = new BoxCollider();

	loadCollider();
}

Player::~Player()
{
	delete bodyCollider;
	delete swordCollider;
}

void Player::Update()
{
	if (!isInitialize)
	{
		initialize();
		isInitialize = true;
	}

	setColliders();

	// �ǰݿ� �ö��̴�
	bodyCollider->Update();

	// Į�� �ö��̴�
	swordCollider->Update();

	input();

	checkNormalAttackCollision(); // �⺻���� ���� �浹üũ.

	UpdateWorld();
	ModelAnimator::Update();
}

void Player::Render()
{
	bodyCollider->Render();
	swordCollider->Render();

	SetWorldBuffer();
	ModelAnimator::Render();
}



void Player::input()
{
	if (Environment::Get()->GetIsTargetCamera())
	{
		move();
	}
	else
	{
		worldCameraMove();
	}
	

	if (KEY_DOWN(VK_LBUTTON))
	{
		normalAttack();
	}
}

void Player::move()
{
	if (mIsNormalAttack) return;

	float terrainY = terrain->GetHeight(mPosition);

	mPosition.y = terrainY;
	
	if (KEY_PRESS('W'))
	{
		rotate();

		mPosition.z += CAMERA->cameraForward.z * mMoveSpeed * DELTA * 1.0f;
		mPosition.x += CAMERA->cameraForward.x * mMoveSpeed * DELTA * 1.0f;

		setAnimation(RUN);
	}

	if (KEY_PRESS('S'))
	{

		rotate();
		mPosition.z += CAMERA->cameraForward.z * -mMoveSpeed * DELTA * 1.0f;
		mPosition.x += CAMERA->cameraForward.x * -mMoveSpeed * DELTA * 1.0f;

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

void Player::worldCameraMove()
{
	if (mIsNormalAttack) return;

	float terrainY = terrain->GetHeight(mPosition);

	mPosition.y = terrainY;

	if (KEY_PRESS('W'))
	{
		mPosition.z += -Forward().z * mMoveSpeed * DELTA;
		mPosition.x += -Forward().x * mMoveSpeed * DELTA;

		setAnimation(RUN);
	}

	if (KEY_PRESS('S'))
	{
		mPosition.z += Forward().z * -mMoveSpeed * DELTA;
		mPosition.x += Forward().x * -mMoveSpeed * DELTA;

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
	Vector3 temp = Vector3::Cross(CAMERA->cameraForward, -1 * Forward());

	if (temp.y < 0.0f) // �÷��̾� �����庤�Ͱ� ī�޶������庤���� ���ʿ� �ִٸ�. // �� ���������� ȸ���ؾ��Ѵٸ�
	{
		if (CompareFloat(CAMERA->cameraForward.x, -Forward().x) &&
			CompareFloat(CAMERA->cameraForward.z, -Forward().z))
		{	}
		else
		{
			mRotation.y += 0.02f;
		}
	}

	else if(temp.y >= 0.0f)// �������� ȸ���ؾ� �Ѵٸ�.
	{
		if (CompareFloat(CAMERA->cameraForward.x, -Forward().x) &&
			CompareFloat(CAMERA->cameraForward.z, -Forward().z))
		{	}
		else
		{
			mRotation.y -= 0.02f;
		}
	}
}

void Player::checkNormalAttackCollision()
{
	if (mIsNormalAttack) // ���ݵ����̸�.
	{
		monsters = GM->GetMonsters();
		for (int i = 0; i < monsters.size(); i++)
		{
			if (!(GM->GetHitCheckMap()[monsters[i]])) // ���ݹ������ִ� ��Ȳ�̸�(�������ӵ� ���� ���ݹ��� �ʾҴٸ�
			{
				if (swordCollider->Collision(monsters[i]->GetHitCollider())) // ��� �浹��üũ.
				{
					monsters[i]->OnDamage(normalAttackDamage);
					check1 = true; // ������.

					//char buff[100];
					//sprintf_s(buff, "�ǳ�\n");
					//OutputDebugStringA(buff);
				}
			}
		}
	}
	
}

void Player::PostRender()
{
	ImGui::Text("isNormalAttack : %d\n ", mIsNormalAttack);
	ImGui::Text("check1 : %d\n ",check1);

	for (int i = 0; i < GM->GetMonsters().size(); i++)
	{
		ImGui::Text("monster %d : %d ", i, GM->GetHitCheckMap()[GM->GetMonsters()[i]]);
	}

}



void Player::setAttackEnd()
{
	setAnimation(IDLE);
	mIsNormalAttack = false;
	check1 = false;
}

void Player::normalAttack()
{
	if (mIsNormalAttack) return;
	setAnimation(ATTACK);
	mIsNormalAttack = true;
}

















void Player::setColliders()
{
	int swordIndex = GetNodeByName("Sword_joint");
	swordMatrix = GetTransformByNode(swordIndex) * mWorldMatrix;
	swordCollider->SetParent(&swordMatrix);

	int bodyIndex = GetNodeByName("Spine");
	bodyMatrix = GetTransformByNode(bodyIndex) * mWorldMatrix;
	bodyCollider->SetParent(&bodyMatrix);
	
}

void Player::loadCollider()
{
	BinaryReader colliderReader(L"TextData/Player.map");
	UINT colliderSize = colliderReader.UInt();

	temp_colliderDatas.resize(colliderSize);
	colliderDatas.resize(colliderSize);
	
	
	void* ptr1 = (void*)temp_colliderDatas.data();

	for (int i = 0; i < colliderSize; i++)
	{
		colliderDatas[i].name = colliderReader.String();
	}


	colliderReader.Byte(&ptr1, sizeof(temp_colliderData) * colliderSize);

	for (int i = 0; i < colliderSize; i++)
	{
		colliderDatas[i].position = temp_colliderDatas[i].position;
		colliderDatas[i].rotation = temp_colliderDatas[i].rotation;
		colliderDatas[i].scale = temp_colliderDatas[i].scale;
	}


	findCollider("SwordCollider", swordCollider);
	findCollider("BodyCollider", bodyCollider);
}

void Player::findCollider(string name, Collider* collider)
{
	for (int i = 0; i < colliderDatas.size(); i++)
	{
		if (colliderDatas[i].name == name)
		{
			collider->mPosition = colliderDatas[i].position;
			collider->mRotation = colliderDatas[i].rotation;
			collider->mScale = colliderDatas[i].scale;
		}
	}
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