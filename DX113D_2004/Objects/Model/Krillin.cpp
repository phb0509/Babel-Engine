#include "Framework.h"

Krillin::Krillin() : ModelAnimator("Krillin/Krillin"),
	moveSpeed(30), rotSpeed(10), isMove(false)
{
	SetShader(L"ModelAnimation");

	ReadClip("Krillin/Idle0");
	ReadClip("Krillin/Run0");
	ReadClip("Krillin/Attack0");

	SetEndEvent(ATTACK, bind(&Krillin::AttackEnd, this));

	SetAction(IDLE);

	//offset.rotation.y = XM_PI;
	//offset.UpdateWorld();
	//parent = offset.GetWorld();

	collider = new CapsuleCollider(1.0f, 2.5f);
	collider->SetParent(GetWorld());
	collider->mPosition.y = 2.5f;
}

Krillin::~Krillin()
{
	delete collider;
}

void Krillin::Update()
{
	KeyboardMove();
	MouseMove();
	Attack();

	UpdateWorld();
	ModelAnimator::Update();
}

void Krillin::Render()
{
	SetWorldBuffer();

	ModelAnimator::Render();	
}

void Krillin::KeyboardMove()
{
	if (KEY_PRESS(VK_UP))
	{
		mPosition -= Forward() * moveSpeed * DELTA;
		SetAction(RUN);
	}
	if (KEY_PRESS(VK_DOWN))
	{
		mPosition += Forward() * moveSpeed * DELTA;
		SetAction(RUN);
	}
	if (KEY_PRESS(VK_RIGHT))
		mRotation.y += rotSpeed * DELTA;
	if (KEY_PRESS(VK_LEFT))
		mRotation.y -= rotSpeed * DELTA;

	if (KEY_UP(VK_UP) || KEY_UP(VK_DOWN))
		SetAction(IDLE);

	mPosition.y = terrain->GetHeight(mGlobalPosition);
}

void Krillin::MouseMove()
{
	if (KEY_DOWN(VK_LBUTTON))
	{
		terrain->ComputePicking(&destPos);
		direction = destPos - mGlobalPosition;
		direction.y = 0.0f;
		direction.Normalize();
		isMove = true;
		SetAction(RUN);

		mRotation.y = atan2(direction.x, direction.z) + XM_PI;
	}

	if (!isMove)
		return;

	mPosition += direction * moveSpeed * DELTA;

	float distance = (mPosition - destPos).Length();

	if (distance < 5.0f)
	{
		SetAction(IDLE);
		isMove = false;
	}
}

void Krillin::Attack()
{
	if (KEY_DOWN(VK_SPACE))
		SetAction(ATTACK);
}

void Krillin::AttackEnd()
{
	SetAction(IDLE);
}

void Krillin::SetAction(State state)
{
	if (this->state != state)
	{
		this->state = state;
		PlayClip(state, 1.0f, 0.1f);
	}
}
