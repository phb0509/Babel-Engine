#include "Framework.h"

Ken::Ken()
	: ModelAnimator("character/character"), moveSpeed(20.0f), rotSpeed(2.0f),
	state(IDLE), accelation(30.0f), deceleration(10.0f),
	jumpPower(0), gravity(98.0f),
	isJump(false), isShoot(false)
{
	mScale = { 0.05f, 0.05f, 0.05f };
	SetShader(L"ModelAnimation");
	SetDiffuseMap(L"Textures/Character/survivorMaleB.png");

	ReadClip("Character/Idle1");
	ReadClip("Character/Run1");
	ReadClip("Character/Jump1");	
	ReadClip("Character/Attack0");

	SetEndEvent(SHOOT, bind(&Ken::SetIdle, this));

	PlayClip(0);

	weapon = new ModelObject("Sword/Sword", Collider::BOX);	
	weapon->mRotation.y = 1.5f;

	Vector3 minBox, maxBox;
	SetBox(&minBox, &maxBox);
	collider = new CapsuleCollider(50, 300);
	collider->mPosition.y = 200;	
	collider->mPosition.z = -30;

	collider->SetParent(&mWorldMatrix);
}

Ken::~Ken()
{
	delete weapon;
	delete collider;
}

void Ken::Update()
{
	if (collider->Collision(weapon->GetCollider()))
	{
		collider->SetColor({ 1, 0, 0, 1 });
	}
	else
	{
		collider->SetColor({ 0, 1, 0, 1 });
	}

	Control();
	Move();
	Rotate();
	Jump();
	Shoot();

	ModelAnimator::Update();		
}

void Ken::Render()
{
	SetWorldBuffer();
	ModelAnimator::Render();

	weapon->Render();
	collider->Render();
}

void Ken::PostRender()
{
	ImGui::SliderFloat3("Pos", (float*)&weapon->mPosition, -30, 30);
	ImGui::SliderFloat3("Rot", (float*)&weapon->mRotation, 0, XM_2PI);
	ImGui::SliderFloat3("Scale", (float*)&weapon->mScale, -30, 30);
}

void Ken::Control()
{
	if (isShoot)
		return;

	if (KEY_PRESS('W'))
	{	
		if(velocity.Length() < MAX_SPEED)
			velocity -= Forward() * accelation * DELTA;
	}
	
	if (KEY_PRESS('D'))
	{
		mRotation.y += rotSpeed * DELTA;
	}
	if (KEY_PRESS('A'))
	{
		mRotation.y -= rotSpeed * DELTA;
	}

	if (!isJump && KEY_DOWN(VK_SPACE))
	{
		jumpPower = JUMP_POWER;
		isJump = true;
		SetAnimation(JUMP);
	}

	if (!isShoot && KEY_DOWN(VK_LBUTTON))
	{
		isShoot = true;
		SetAnimation(SHOOT);
	}

	if (KEY_DOWN(VK_RBUTTON))
	{
		path.clear();

		terrain->ComputePicking(&destPos);

		Ray ray;
		ray.position = mPosition;
		ray.direction = (destPos - mPosition).Normal();

		float distance = Distance(destPos, mPosition);

		if (aStar->CollisionObstacle(ray, distance))
		{
			int startIndex = aStar->FindCloseNode(mPosition);
			int endIndex = aStar->FindCloseNode(destPos);

			path = aStar->FindPath(startIndex, endIndex);
			path.insert(path.begin(), destPos);

			aStar->MakeDirectPath(mPosition, destPos, path);
			path.insert(path.begin(), destPos);

			UINT pathSize = path.size();

			while (path.size() > 2)
			{
				vector<Vector3> tempPath;
				for (UINT i = 1; i < path.size() - 1; i++)
					tempPath.emplace_back(path[i]);

				Vector3 start = path.back();
				Vector3 end = path.front();

				aStar->MakeDirectPath(start, end, tempPath);

				path.clear();
				path.emplace_back(end);

				for (Vector3 temp : tempPath)
					path.emplace_back(temp);

				path.emplace_back(start);

				if (pathSize == path.size())
					break;
				else
					pathSize = path.size();
			}
		}
		else
		{
			path.insert(path.begin(), destPos);
		}
	}
}

void Ken::Move()
{
	MovePath();

	if (velocity.Length() > 0.0f)
	{
		Vector3 zero;
		velocity = LERP(velocity, zero, deceleration * DELTA);
	}

	mPosition += velocity * moveSpeed * DELTA;

	if (isJump || isShoot)
		return;

	if (velocity.Length() > 0.5f)	{

		SetAnimation(RUN);		
	}
	else
	{
		SetAnimation(IDLE);
	}	
}

void Ken::MovePath()
{
	if (path.empty())
		return;

	Vector3 dest = path.back();

	Vector3 direction = dest - mPosition;

	if (direction.Length() < 0.1f)
		path.pop_back();

	velocity = direction.Normal();
}

void Ken::Rotate()
{
	if (velocity.Length() < 0.1f)
		return;

	Vector3 start = Forward() * -1.0f;
	Vector3 end = velocity.Normal();

	float cosValue = Vector3::Dot(start, end);
	float angle = acos(cosValue);

	if (angle < 0.1f)
		return;

	Vector3 cross = Vector3::Cross(start, end);

	if (cross.y > 0.0f)
		mRotation.y += rotSpeed * DELTA;
	else
		mRotation.y -= rotSpeed * DELTA;
}

void Ken::Jump()
{
	float terrainY = terrain->GetHeight(mPosition);

	if (!isJump)
	{
		mPosition.y = terrainY;
		return;
	}

	jumpPower -= gravity * DELTA;

	mPosition.y += jumpPower * DELTA;

	if (mPosition.y <= terrainY)
	{
		SetAnimation(IDLE);
		isJump = false;
		mPosition.y = terrainY;
		isShoot = false;
	}
}

void Ken::Shoot()
{
	SetWeapon();
}

void Ken::SetIdle()
{
	SetAnimation(IDLE);
	isShoot = false;
}

void Ken::SetAnimation(State value)
{
	if (state != value)
	{
		state = value;
		PlayClip(state);
	}
}

void Ken::SetWeapon()
{
	int index = GetNodeByName("RightHand");

	rightHand = GetTransformByNode(index) * mWorldMatrix;

	weapon->SetParent(&rightHand);
}
