#include "Framework.h"

Camera::Camera():
	mMoveSpeed(50.0f),
	mRotationSpeed(2.0f),
	mWheelSpeed(5.0f),
	mbIsInitialized(false),
	mDistanceToNearZ(0.1f),
	mDistanceToFarZ(1000.0f),
	mAspectRatio(WIN_WIDTH/(float)WIN_HEIGHT),
	mFoV (XM_PIDIV4),
	mbIsPerspectiveProjection(true),
	mbIsUsingFrustumCulling(false),
	mbIsRenderFrustumCollider(false)
{
	mViewBuffer = new ViewBuffer();
	mFrustum = new Frustum(mFoV,mAspectRatio,mDistanceToNearZ,mDistanceToFarZ);

	createPerspectiveProjectionBuffer(mFoV, mAspectRatio, mDistanceToNearZ, mDistanceToFarZ);
	createOrthographicProjectionBuffer(mDistanceToNearZ, mDistanceToFarZ);
}

Camera::~Camera()
{
	GM->SafeDelete(mPerspectiveProjectionBuffer);
	GM->SafeDelete(mOrthographicProjectionBuffer);
	GM->SafeDelete(mViewBuffer);
	GM->SafeDelete(mFrustum);
}

void Camera::Update()
{
	Transform::UpdateWorld();

	if (!mbIsInitialized)
	{
		initialize();
		mbIsInitialized = true;
	}

	if (mbIsUsingFrustumCulling)
	{
		mFrustum->Update();
	}
}

void Camera::Render()
{
}

void Camera::PostRender()
{
}

void Camera::RenderFrustumCollider()
{
	if (mbIsUsingFrustumCulling)
	{
		if (mbIsRenderFrustumCollider)
		{
			mFrustum->RenderCollider();
		}
	}
}

void Camera::initialize()
{
	mFrustum->SetCamera(this);
}

void Camera::SetViewToFrustum(Matrix view)
{
	mFrustum->SetView(view);
}

void Camera::SetViewMatrixToBuffer()
{
	Transform::UpdateWorld();

	Vector3 focus = mPosition + GetForwardVector();
	mViewMatrix = XMMatrixLookAtLH(mPosition.data, focus.data, GetUpVector().data); // 카메라위치, 타겟위치, 카메라 윗벡터
	mViewBuffer->SetMatrix(mViewMatrix);
}

void Camera::SetViewBufferToVS(UINT slot) // Default slot1
{
	mViewBuffer->SetVSBuffer(slot);
}

void Camera::SetViewBufferToPS(UINT slot)
{
	mViewBuffer->SetPSBuffer(slot);
}

void Camera::SetProjectionBufferToVS(UINT slot) // Default slot2
{
	if (mbIsPerspectiveProjection)
	{
		mPerspectiveProjectionBuffer->SetVSBuffer(slot);
	}
	else
	{
		mOrthographicProjectionBuffer->SetVSBuffer(slot);
	}
}

void Camera::SetProjectionBufferToPS(UINT slot)
{
	if (mbIsPerspectiveProjection)
	{
		mPerspectiveProjectionBuffer->SetPSBuffer(slot);
	}
	else
	{
		mOrthographicProjectionBuffer->SetPSBuffer(slot);
	}
}

void Camera::SetProjectionOption(float FoV, float aspectRatio, float distanceToNearZ, float distanceToFarZ)
{
	mFoV = FoV;
	mAspectRatio = aspectRatio;
	mDistanceToNearZ = distanceToNearZ; 
	mDistanceToFarZ = distanceToFarZ; 

	delete mFrustum;
	mFrustum = new Frustum(mFoV, mAspectRatio, mDistanceToNearZ, mDistanceToFarZ);

	createPerspectiveProjectionBuffer(mFoV, mAspectRatio, mDistanceToNearZ, mDistanceToFarZ);
	createOrthographicProjectionBuffer(mDistanceToNearZ, mDistanceToFarZ);
}

void Camera::SetFoV(float fov)
{
	mFoV = fov;

	delete mFrustum;
	mFrustum = new Frustum(mFoV, mAspectRatio, mDistanceToNearZ, mDistanceToFarZ);
	createPerspectiveProjectionBuffer(mFoV, mAspectRatio, mDistanceToNearZ, mDistanceToFarZ);
}

void Camera::SetAspectRatio(float aspectRatio)
{
	mAspectRatio = aspectRatio;

	delete mFrustum;
	mFrustum = new Frustum(mFoV, mAspectRatio, mDistanceToNearZ, mDistanceToFarZ);
	createPerspectiveProjectionBuffer(mFoV, mAspectRatio, mDistanceToNearZ, mDistanceToFarZ);
}

void Camera::SetDistanceToNearZ(float distanceToNearZ)
{
	mDistanceToNearZ = distanceToNearZ;

	delete mFrustum;
	mFrustum = new Frustum(mFoV, mAspectRatio, mDistanceToNearZ, mDistanceToFarZ);
	createPerspectiveProjectionBuffer(mFoV, mAspectRatio, mDistanceToNearZ, mDistanceToFarZ);
}

void Camera::SetDistanceToFarZ(float distanceToFarZ)
{
	mDistanceToFarZ = distanceToFarZ;

	delete mFrustum;
	mFrustum = new Frustum(mFoV, mAspectRatio, mDistanceToNearZ, mDistanceToFarZ);
	createPerspectiveProjectionBuffer(mFoV, mAspectRatio, mDistanceToNearZ, mDistanceToFarZ);
}

Ray Camera::ScreenPointToRay(Vector3 pos) // 마우스좌표 받음.
{
	Float2 screenSize(WIN_WIDTH, WIN_HEIGHT);

	Float2 point;
	point.x = ((2 * pos.x) / screenSize.x) - 1.0f; // 마우스위치값을 -1~1로 정규화. NDC좌표로 변환.
	point.y = (((2 * pos.y) / screenSize.y) - 1.0f) * -1.0f;                        
	
	Float4x4 temp;

	if (mbIsPerspectiveProjection)
	{
		XMStoreFloat4x4(&temp, mPerspectiveProjectionMatrix);
	}
	else
	{
		XMStoreFloat4x4(&temp, mOrthographicProjectionMatrix);
	}
	
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

void Camera::Move()
{
	// Update Position
	if (KEY_PRESS(VK_RBUTTON))
	{
		if (KEY_PRESS('I'))
			mPosition += GetForwardVector() * mMoveSpeed * DELTA;
		if (KEY_PRESS('K'))
			mPosition -= GetForwardVector() * mMoveSpeed * DELTA;
		if (KEY_PRESS('J'))
			mPosition -= GetRightVector() * mMoveSpeed * DELTA;
		if (KEY_PRESS('L'))
			mPosition += GetRightVector() * mMoveSpeed * DELTA;
		if (KEY_PRESS('U'))
			mPosition -= GetUpVector() * mMoveSpeed * DELTA;
		if (KEY_PRESS('O'))
			mPosition += GetUpVector() * mMoveSpeed * DELTA;
	}

	mPosition += GetForwardVector() * Control::Get()->GetWheel() * mWheelSpeed * DELTA;

	// Update Rotation
	if (KEY_PRESS(VK_RBUTTON))
	{
		Vector3 value = MOUSEPOS - mPreFrameMousePosition;

		mRotation.x += value.y * mRotationSpeed * DELTA;
		mRotation.y += value.x * mRotationSpeed * DELTA;
	}

	mPreFrameMousePosition = MOUSEPOS;
	SetViewMatrixToBuffer();
}

Matrix Camera::GetProjectionMatrixInUse()
{
	if (mbIsPerspectiveProjection)
	{
		return mPerspectiveProjectionMatrix;
	}

	return mOrthographicProjectionMatrix;
}

ProjectionBuffer* Camera::GetProjectionBufferInUse()
{
	if (mbIsPerspectiveProjection)
	{
		return mPerspectiveProjectionBuffer;
	}

	return mOrthographicProjectionBuffer;
}

void Camera::createPerspectiveProjectionBuffer(float fov,float aspectRatio, float distanceToNearZ, float distanceToFarZ)
{
	if (mPerspectiveProjectionBuffer != nullptr)
	{
		delete mPerspectiveProjectionBuffer;
	}

	mPerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, distanceToNearZ, distanceToFarZ);
	mPerspectiveProjectionBuffer = new ProjectionBuffer();
	mPerspectiveProjectionBuffer->SetMatrix(mPerspectiveProjectionMatrix);
}

void Camera::createOrthographicProjectionBuffer(float distanceToNearZ, float distanceToFarZ)
{
	if (mOrthographicProjectionBuffer != nullptr)
	{
		delete mOrthographicProjectionBuffer;
	}

	mOrthographicProjectionMatrix = XMMatrixOrthographicLH(WIN_WIDTH, WIN_HEIGHT, distanceToNearZ, distanceToFarZ);
	mOrthographicProjectionBuffer = new ProjectionBuffer();
	mOrthographicProjectionBuffer->SetMatrix(mOrthographicProjectionMatrix);
}


