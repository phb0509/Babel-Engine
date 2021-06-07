#include "Framework.h"

Camera::Camera()
	:
	moveSpeed(50.0f),
	mRotationSpeed(2.0f),
	distance(13),
	height(11),
	offset(0, 5, 0),
	moveDamping(5),
	rotDamping(0),
	destRot(0),
	rotY(0.0f),
	rotX(0.0f),
	target(nullptr),
	wheelSpeed(15.0f),
	mbIsOnFrustumCollider(false),
	mbIsMouseInputing(true),
	mbHasInitalized(false)

{
	mViewBuffer = new ViewBuffer();
	mFrustum = new Frustum();
	
	oldPos = MOUSEPOS;
}

Camera::~Camera()
{
	delete mViewBuffer;
}

void Camera::Update()
{
	if (!mbHasInitalized)
	{
		initialize();
		mbHasInitalized = true;
	}


	if (target != nullptr)
	{
		if (mbIsMouseInputing)
		{
			targetMove();
		}
		else
		{
			targetMoveInWorldCamera();
		}
	
		mFrustum->Update();
	}
	else
	{
		freeMode();
	}
}

void Camera::Render()
{
	if (target != nullptr)
	{
		mFrustum->Render();
	}
}



void Camera::targetMove()
{
	mRotation = { 0.0f,0.0f,0.0f };
	followControl();

	mRotMatrixY = XMMatrixRotationY(rotY); // rotY는 FollowControl에서 마우스x좌표값 이동절대값에 따라 크기조절. 걍 rotY만큼 이동량 조절.
	mRotMatrixX = XMMatrixRotationX(rotX);

	Vector3 forward = XMVector3TransformNormal(kForward, mRotMatrixX * mRotMatrixY); // rotMatrix의 방향만? 따오는듯.
	//forward = XMVector3TransformNormal(forward, mRotMatrixX);

	destPos = forward * -distance;
	destPos += target->GetGlobalPosition();
	destPos.y += height;

	mPosition = LERP(mPosition, destPos, 5.0f * DELTA); // 카메라 위치.
													  // 현위치에서 desPost까지 moveDamping값만큼.

	Vector3 tempOffset = XMVector3TransformCoord(offset.data, mRotMatrixY);
	Vector3 targetPosition = target->GetGlobalPosition() + tempOffset;

	//cameraForward = (player->GlobalPos() - position).Normal();
	cameraForward = forward.Normal();

	mViewMatrix = XMMatrixLookAtLH(mPosition.data, targetPosition.data,
		Up().data); // 카메라위치 , 타겟위치 , 카메라의 업벡터
	mViewBuffer->Set(mViewMatrix);
	setViewToFrustum(mViewMatrix);
	
}

void Camera::targetMoveInWorldCamera()
{
	Vector3 targetPosition = target->mPosition;
	mPosition = target->mPosition + target->Forward() * 1.1f;
	


	mViewMatrix = XMMatrixLookAtLH(mPosition.data, targetPosition.data,
		Up().data); // 카메라위치 , 타겟위치 , 카메라의 업벡터
	mViewBuffer->Set(mViewMatrix);
	setViewToFrustum(mViewMatrix);
}



void Camera::followControl()
{
	// 마우스를 계속 좌측이나 우측으로 돌릴 시, 계속 캐릭터 회전시키기 위해
	// 끝에 닿을 시 커서좌표 이동시켜야함.
	// 버그많음.고쳐야됨.

	/*if (MOUSEPOS.x >= WIN_WIDTH-20)
	{
		ClientToScreen(hWnd, &pt);

		pt.x = WIN_WIDTH - 100;
		pt.y = MOUSEPOS.y;

		SetCursorPos(pt.x, pt.y);

		oldPos.x = pt.x;
		oldPos.y = pt.y;
	}

	if (MOUSEPOS.x <= 0 + 20)
	{
		ClientToScreen(hWnd, &pt);

		pt.x = 100;
		pt.y = MOUSEPOS.y;

		SetCursorPos(pt.x, pt.y);

		oldPos.x = pt.x;
		oldPos.y = pt.y;
	}*/


	Vector3 value = MOUSEPOS - oldPos;

	rotY += value.x * mRotationSpeed * DELTA;
	rotX += value.y * mRotationSpeed * DELTA;

	oldPos = MOUSEPOS;
}



void Camera::freeMode()
{
	freeMove();
	rotation();
	SetViewMatrixToBuffer();
}



void Camera::freeMove()
{
	if (KEY_PRESS(VK_RBUTTON))
	{
		if (KEY_PRESS('I'))
			mPosition += Forward() * moveSpeed * DELTA;
		if (KEY_PRESS('K'))
			mPosition -= Forward() * moveSpeed * DELTA;
		if (KEY_PRESS('J'))
			mPosition -= Right() * moveSpeed * DELTA;
		if (KEY_PRESS('L'))
			mPosition += Right() * moveSpeed * DELTA;
		if (KEY_PRESS('U'))
			mPosition -= Up() * moveSpeed * DELTA;
		if (KEY_PRESS('O'))
			mPosition += Up() * moveSpeed * DELTA;
	}

	mPosition += Forward() * Control::Get()->GetWheel() * wheelSpeed * DELTA;
}

void Camera::rotation()
{
	if (KEY_PRESS(VK_RBUTTON))
	{
		Vector3 value = MOUSEPOS - oldPos;

		mRotation.x += value.y * mRotationSpeed * DELTA;
		mRotation.y += value.x * mRotationSpeed * DELTA;
	}

	oldPos = MOUSEPOS;
}

void Camera::initialize()
{
	mFrustum->SetCamera(this);
}

void Camera::SetViewMatrixToBuffer()
{
	UpdateWorld();

	Vector3 focus = mPosition + Forward();
	mViewMatrix = XMMatrixLookAtLH(mPosition.data, focus.data, Up().data); // 카메라위치, 타겟위치, 카메라 윗벡터

	mViewBuffer->Set(mViewMatrix);
}




void Camera::PostRender()
{
	ImGui::Text("CameraInfo");
	ImGui::Text("CamPos : %.1f, %.1f, %.1f", mPosition.x, mPosition.y, mPosition.z);
	ImGui::Text("CamRot : %.1f, %.1f, %.1f", mRotation.x, mRotation.y, mRotation.z);
	ImGui::Text("CameraForward : %.3f, %.3f, %.3f", cameraForward.x, cameraForward.y, cameraForward.z);
	ImGui::Text("MousePosition : %.1f, %.1f", MOUSEPOS.x, MOUSEPOS.y);


	if (target == nullptr)
	{
		ImGui::SliderFloat("MoveSpeed", &moveSpeed, 0, 100);
		ImGui::SliderFloat("RotSpeed", &mRotationSpeed, 0, 10);
	}
	else
	{
		ImGui::SliderFloat("CamDistance", &distance, -10.0f, 100.0f);
		ImGui::SliderFloat("CamHeight", &height, -10.0f, 100.0f);
		ImGui::SliderFloat("CamMoveDamping", &moveDamping, 0.0f, 30.0f);
		ImGui::SliderFloat("CamRotDamping", &rotDamping, 0.0f, 30.0f);
		ImGui::SliderFloat3("CamOffset", (float*)&offset, -20.0f, 20.0f);
	}
	ImGui::Spacing();
}

void Camera::SetVS(UINT slot)
{
	mViewBuffer->SetVSBuffer(slot);
}

Ray Camera::ScreenPointToRay(Vector3 pos) // 마우스좌표 받음.
{
	Float2 screenSize(WIN_WIDTH, WIN_HEIGHT);

	Float2 point;
	point.x = ((2 * pos.x) / screenSize.x) - 1.0f; // 마우스위치값을 -1~1로 정규화.
	point.y = (((2 * pos.y) / screenSize.y) - 1.0f) * -1.0f;

	Matrix projection = Environment::Get()->GetProjection();

	Float4x4 temp;
	XMStoreFloat4x4(&temp, projection);

	point.x /= temp._11;
	point.y /= temp._22;

	Ray ray;
	ray.position = mPosition;

	Matrix invView = XMMatrixInverse(nullptr, mViewMatrix);

	Vector3 tempPos(point.x, point.y, 1.0f);

	ray.direction = XMVector3TransformNormal(tempPos.data, invView);
	ray.direction.Normalize();

	return ray;
}

void Camera::setViewToFrustum(Matrix view)
{
	mFrustum->SetView(view);
}


