#include "Framework.h"

Camera::Camera()
	:
	mMoveSpeed(50.0f),
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
	mWheelSpeed(5.0f),
	mbIsOnFrustumCollider(false),
	mbIsMouseInputing(true),
	mbHasInitalized(false),
	mFrustum(nullptr)

{
	mViewBuffer = new ViewBuffer();

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
		if (mbIsMouseInputing) // 1인칭시점이 타겟카메라시점일 때
		{
			targetMove(); // 타겟카메라시점의 타겟카메라이동
		}
		else
		{
			targetMoveInWorldCamera(); // 1인칭 시점이 월드카메라 시점일때 타겟카메라의 이동, 프러스텀컬링 확인용.
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

void Camera::PostRender()
{
	if (mFrustum != nullptr)
	{
		mFrustum->PostRender();
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
	mCameraForward = forward.Normal();

	mViewMatrix = XMMatrixLookAtLH(mPosition.data, targetPosition.data,
		Up().data); // 카메라위치 , 타겟위치 , 카메라의 업벡터

	// 프러스텀에 뷰버퍼 설정.
	mViewBuffer->Set(mViewMatrix);
	setViewToFrustum(mViewMatrix);
	
}

void Camera::targetMoveInWorldCamera()
{
	Vector3 targetPosition = target->mPosition;
	Vector3 beforePosition = mPosition;

	mPosition = target->mPosition + target->Forward() * 1.1f;
	mDistanceToTarget = (mPosition - beforePosition).Length();

	mViewMatrix = XMMatrixLookAtLH(mPosition.data, targetPosition.data,
		Up().data); // 카메라위치 , 타겟위치 , 카메라의 업벡터

	// 프러스텀에 뷰버퍼 설정.
	mViewBuffer->Set(mViewMatrix);
	setViewToFrustum(mViewMatrix);

	//Vector3 targetPosition = target->mPosition;
	//mPosition = target->mPosition + target->Forward() * 1.1f; // 타겟의 뒤.(모델은 포워드벡터가 반대다)

	//RotateToDestinationForNotModel(this, targetPosition);

	//Vector3 focus = mPosition + Forward();

	//char buff[100];
	//sprintf_s(buff, "Forwrd.x : %f  Forward.y : %f  Forward.z : %f\n", Forward().x,Forward().y,Forward().z);
	//OutputDebugStringA(buff);

	//mViewMatrix = XMMatrixLookAtLH(mPosition.data, focus.data,
	//	Up().data); // 카메라위치 , 타겟위치 , 카메라의 업벡터

	//mViewBuffer->Set(mViewMatrix);
	//setViewToFrustum(mViewMatrix);
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
			mPosition += Forward() * mMoveSpeed * DELTA;
		if (KEY_PRESS('K'))
			mPosition -= Forward() * mMoveSpeed * DELTA;
		if (KEY_PRESS('J'))
			mPosition -= Right() * mMoveSpeed * DELTA;
		if (KEY_PRESS('L'))
			mPosition += Right() * mMoveSpeed * DELTA;
		if (KEY_PRESS('U'))
			mPosition -= Up() * mMoveSpeed * DELTA;
		if (KEY_PRESS('O'))
			mPosition += Up() * mMoveSpeed * DELTA;
	}

	mPosition += Forward() * Control::Get()->GetWheel() * mWheelSpeed * DELTA;
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
	if(mFrustum != nullptr)
	{
		mFrustum->SetCamera(this);
	}
}

void Camera::SetViewMatrixToBuffer()
{
	UpdateWorld();

	Vector3 focus = mPosition + Forward();
	mViewMatrix = XMMatrixLookAtLH(mPosition.data, focus.data, Up().data); // 카메라위치, 타겟위치, 카메라 윗벡터
	mViewBuffer->Set(mViewMatrix);
}




void Camera::SetVS(UINT slot)
{
	mViewBuffer->SetVSBuffer(slot);
}

Ray Camera::ScreenPointToRay(Vector3 pos) // 마우스좌표 받음.
{
	Float2 screenSize(WIN_WIDTH, WIN_HEIGHT);

	Float2 point;
	point.x = ((2 * pos.x) / screenSize.x) - 1.0f; // 마우스위치값을 -1~1로 정규화. NDC좌표로 변환.
	point.y = (((2 * pos.y) / screenSize.y) - 1.0f) * -1.0f;

	Matrix projection = Environment::Get()->GetProjection();

	Float4x4 temp;
	XMStoreFloat4x4(&temp, projection);

	//Unprojection
	point.x /= temp._11; // 뷰공간의 x,y좌표로 전환.
	point.y /= temp._22;

	
	Ray ray;
	ray.position = mPosition;

	Matrix invView = XMMatrixInverse(nullptr, mViewMatrix);
	
	Vector3 tempPos(point.x, point.y, 1.0f);

	ray.direction = XMVector3TransformNormal(tempPos.data, invView);
	ray.direction.Normalize();

	return ray;
}

void Camera::CreateFrustum()
{
	mFrustum = new Frustum();
}

void Camera::setViewToFrustum(Matrix view)
{
	mFrustum->SetView(view);
}


