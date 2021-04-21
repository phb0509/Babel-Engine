#include "Framework.h"

Player::Player()
	: ModelAnimator("Player/Player"), 
	isInitialize(false), 
	mMoveSpeed(50.0f),
	state(IDLE), 
	isNormalAttack(false),
	isNormalAttackCollide(false), 
	normalAttackDamage(10.0f)
{
	scale = { 0.05f, 0.05f, 0.05f };
	

	SetShader(L"ModelAnimation");

	ReadClip("Player/TPose0.clip");
	ReadClip("Player/Idle0.clip");
	ReadClip("Player/Run0.clip");
	ReadClip("Player/Attack0.clip");
	ReadClip("Player/Die0.clip");

	SetEndEvent(RUN, bind(&Player::SetIdle, this));
	SetEndEvent(ATTACK, bind(&Player::SetAttackEnd, this));


	PlayClip(1);

	rotation.y = XM_PI;

	bodyCollider = new BoxCollider();
	swordCollider = new BoxCollider();

	LoadCollider();

	
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
		Initialize();
		isInitialize = true;
	}

	SetColliders();

	// 피격용 컬라이더
	bodyCollider->Update();

	// 칼날 컬라이더
	swordCollider->Update();

	Input();

	CheckNormalAttackCollision(); // 기본공격 몬스터 충돌체크.

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







void Player::Input()
{
	Move();

	if (KEY_DOWN(VK_LBUTTON))
	{
		NormalAttack();
	}

}

void Player::Move()
{
	if (isNormalAttack) return;

	float terrainY = terrain->GetHeight(position);


	position.y = terrainY;
	
	if (KEY_PRESS('W'))
	{
		Rotate();

		position.z += CAMERA->cameraForward.z * mMoveSpeed * DELTA * 1.0f;
		position.x += CAMERA->cameraForward.x * mMoveSpeed * DELTA * 1.0f;

		SetAnimation(RUN);
	}

	if (KEY_PRESS('S'))
	{

		Rotate();
		position.z += CAMERA->cameraForward.z * -mMoveSpeed * DELTA * 1.0f;
		position.x += CAMERA->cameraForward.x * -mMoveSpeed * DELTA * 1.0f;

		SetAnimation(RUN);
	}

	if (KEY_PRESS('A'))
	{
		Rotate();
		position += Right() * mMoveSpeed * DELTA;
	}

	if (KEY_PRESS('D'))
	{
		Rotate();
		position += Right() * -mMoveSpeed * DELTA;
	}
}

void Player::Rotate()
{
	Vector3 temp = Vector3::Cross(CAMERA->cameraForward, -1 * Forward());

	if (temp.y < 0.0f) // 플레이어 포워드벡터가 카메라포워드벡터의 왼쪽에 있다면. // 즉 오른쪽으로 회전해야한다면
	{
		if (CompareFloat(CAMERA->cameraForward.x, -Forward().x) &&
			CompareFloat(CAMERA->cameraForward.z, -Forward().z))
		{	}
		else
		{
			rotation.y += 0.02f;
		}
	}

	else if(temp.y >= 0.0f)// 왼쪽으로 회전해야 한다면.
	{
		if (CompareFloat(CAMERA->cameraForward.x, -Forward().x) &&
			CompareFloat(CAMERA->cameraForward.z, -Forward().z))
		{	}
		else
		{
			rotation.y -= 0.02f;
		}
	}
}

void Player::CheckNormalAttackCollision()
{
	if (isNormalAttack) // 공격도중이면.
	{
		monsters = GM->GetMonsters();
		for (int i = 0; i < monsters.size(); i++)
		{
			if (!(GM->GetHitCheckMap()[monsters[i]])) // 공격받을수있는 상황이면(한프레임도 아직 공격받지 않았다면
			{
				if (swordCollider->Collision(monsters[i]->GetHitCollider())) // 고놈만 충돌ㅇ체크.
				{
					monsters[i]->OnDamage(normalAttackDamage);
					check1 = true; // 디버깅용.

					//char buff[100];
					//sprintf_s(buff, "되냐\n");
					//OutputDebugStringA(buff);
				}
			}
		}
	}
	
}

void Player::PostRender()
{
	ImGui::Text("isNormalAttack : %d\n ", isNormalAttack);
	ImGui::Text("check1 : %d\n ",check1);

	for (int i = 0; i < GM->GetMonsters().size(); i++)
	{
		ImGui::Text("monster %d : %d ", i, GM->GetHitCheckMap()[GM->GetMonsters()[i]]);
	}

}



void Player::SetAttackEnd()
{
	SetAnimation(IDLE);
	isNormalAttack = false;
	check1 = false;
}

void Player::NormalAttack()
{
	if (isNormalAttack) return;
	SetAnimation(ATTACK);
	isNormalAttack = true;
}

















void Player::SetColliders()
{
	int swordIndex = GetNodeByName("Sword_joint");
	swordMatrix = GetTransformByNode(swordIndex) * world;
	swordCollider->SetParent(&swordMatrix);

	int bodyIndex = GetNodeByName("Spine");
	bodyMatrix = GetTransformByNode(bodyIndex) * world;
	bodyCollider->SetParent(&bodyMatrix);
	
}

void Player::LoadCollider()
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


	FindCollider("SwordCollider", swordCollider);
	FindCollider("BodyCollider", bodyCollider);
}

void Player::FindCollider(string name, Collider* collider)
{
	for (int i = 0; i < colliderDatas.size(); i++)
	{
		if (colliderDatas[i].name == name)
		{
			collider->position = colliderDatas[i].position;
			collider->rotation = colliderDatas[i].rotation;
			collider->scale = colliderDatas[i].scale;
		}
	}
}

void Player::Initialize()
{
	
}

void Player::SetIdle()
{
	SetAnimation(IDLE);
}

void Player::SetAnimation(State value)
{
	if (state != value)
	{
		state = value;
		PlayClip(state);
	}
}