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
		if (mbIsMouseInputing) // 1��Ī������ Ÿ��ī�޶������ ��
		{
			targetMove(); // Ÿ��ī�޶������ Ÿ��ī�޶��̵�
		}
		else
		{
			targetMoveInWorldCamera(); // 1��Ī ������ ����ī�޶� �����϶� Ÿ��ī�޶��� �̵�, ���������ø� Ȯ�ο�.
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

	mRotMatrixY = XMMatrixRotationY(rotY); // rotY�� FollowControl���� ���콺x��ǥ�� �̵����밪�� ���� ũ������. �� rotY��ŭ �̵��� ����.
	mRotMatrixX = XMMatrixRotationX(rotX);

	Vector3 forward = XMVector3TransformNormal(kForward, mRotMatrixX * mRotMatrixY); // rotMatrix�� ���⸸? �����µ�.
	//forward = XMVector3TransformNormal(forward, mRotMatrixX);

	destPos = forward * -distance;
	destPos += target->GetGlobalPosition();
	destPos.y += height;

	mPosition = LERP(mPosition, destPos, 5.0f * DELTA); // ī�޶� ��ġ.
													    // ����ġ���� desPost���� moveDamping����ŭ.

	Vector3 tempOffset = XMVector3TransformCoord(offset.data, mRotMatrixY);
	Vector3 targetPosition = target->GetGlobalPosition() + tempOffset;

	//cameraForward = (player->GlobalPos() - position).Normal();
	mCameraForward = forward.Normal();

	mViewMatrix = XMMatrixLookAtLH(mPosition.data, targetPosition.data,
		Up().data); // ī�޶���ġ , Ÿ����ġ , ī�޶��� ������

	// �������ҿ� ����� ����.
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
		Up().data); // ī�޶���ġ , Ÿ����ġ , ī�޶��� ������

	// �������ҿ� ����� ����.
	mViewBuffer->Set(mViewMatrix);
	setViewToFrustum(mViewMatrix);

	//Vector3 targetPosition = target->mPosition;
	//mPosition = target->mPosition + target->Forward() * 1.1f; // Ÿ���� ��.(���� �����庤�Ͱ� �ݴ��)

	//RotateToDestinationForNotModel(this, targetPosition);

	//Vector3 focus = mPosition + Forward();

	//char buff[100];
	//sprintf_s(buff, "Forwrd.x : %f  Forward.y : %f  Forward.z : %f\n", Forward().x,Forward().y,Forward().z);
	//OutputDebugStringA(buff);

	//mViewMatrix = XMMatrixLookAtLH(mPosition.data, focus.data,
	//	Up().data); // ī�޶���ġ , Ÿ����ġ , ī�޶��� ������

	//mViewBuffer->Set(mViewMatrix);
	//setViewToFrustum(mViewMatrix);
}



void Camera::followControl()
{
	// ���콺�� ��� �����̳� �������� ���� ��, ��� ĳ���� ȸ����Ű�� ����
	// ���� ���� �� Ŀ����ǥ �̵����Ѿ���.
	// ���׸���.���ľߵ�.

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
	mViewMatrix = XMMatrixLookAtLH(mPosition.data, focus.data, Up().data); // ī�޶���ġ, Ÿ����ġ, ī�޶� ������
	mViewBuffer->Set(mViewMatrix);
}




void Camera::SetVS(UINT slot)
{
	mViewBuffer->SetVSBuffer(slot);
}

Ray Camera::ScreenPointToRay(Vector3 pos) // ���콺��ǥ ����.
{
	Float2 screenSize(WIN_WIDTH, WIN_HEIGHT);

	Float2 point;
	point.x = ((2 * pos.x) / screenSize.x) - 1.0f; // ���콺��ġ���� -1~1�� ����ȭ. NDC��ǥ�� ��ȯ.
	point.y = (((2 * pos.y) / screenSize.y) - 1.0f) * -1.0f;

	Matrix projection = Environment::Get()->GetProjection();

	Float4x4 temp;
	XMStoreFloat4x4(&temp, projection);

	//Unprojection
	point.x /= temp._11; // ������� x,y��ǥ�� ��ȯ.
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


