#include "Framework.h"

Camera::Camera():
	mMoveSpeed(50.0f),
	mRotationSpeed(2.0f),
	mWheelSpeed(5.0f),
	mbIsInitialized(false),
	mFrustum(nullptr),
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

	createPerspectiveProjectionBuffer();
	createOrthographicProjectionBuffer();
}

Camera::~Camera()
{
	delete mViewBuffer;
	delete mFrustum;
}

void Camera::Update()
{
	Transform::UpdateWorld();
	//SetViewMatrixToBuffer();

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

	Vector3 focus = mPosition + Forward();
	mViewMatrix = XMMatrixLookAtLH(mPosition.data, focus.data, Up().data); // 카메라위치, 타겟위치, 카메라 윗벡터
	mViewBuffer->SetMatrix(mViewMatrix);
}

void Camera::SetViewBufferToVS(UINT slot)
{
	mViewBuffer->SetVSBuffer(slot);
}

void Camera::SetViewBufferToPS(UINT slot)
{
	mViewBuffer->SetPSBuffer(slot);
}

void Camera::SetProjectionBufferToVS(UINT slot)
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
	createPerspectiveProjectionBuffer();
	createOrthographicProjectionBuffer();
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

void Camera::createPerspectiveProjectionBuffer()
{
	if (mPerspectiveProjectionBuffer != nullptr)
	{
		delete mPerspectiveProjectionBuffer;
	}

	mPerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(mFoV,
		mAspectRatio, mDistanceToNearZ, mDistanceToFarZ);
	mPerspectiveProjectionBuffer = new ProjectionBuffer();
	mPerspectiveProjectionBuffer->SetMatrix(mPerspectiveProjectionMatrix);
}

void Camera::createOrthographicProjectionBuffer()
{
	//float w = 2.0f / WIN_WIDTH;
	//float h = 2.0f / WIN_HEIGHT;
	//float a = 0.001f;
	//float b = -a * 0.001f;

	////float a = 1.0f;
	////float b = 0.0f;

	//Matrix orthographicMatrix = XMMatrixIdentity();

	//orthographicMatrix.r[0].m128_f32[0] = w;
	//orthographicMatrix.r[1].m128_f32[1] = h;
	//orthographicMatrix.r[2].m128_f32[2] = a;
	//orthographicMatrix.r[3].m128_f32[2] = b;
	//orthographicMatrix.r[3].m128_f32[3] = 1;

	//mOrthographicProjectionBuffer = new ProjectionBuffer();
	//mOrthographicProjectionBuffer->Set(orthographicMatrix);

	if (mOrthographicProjectionBuffer != nullptr)
	{
		delete mOrthographicProjectionBuffer;
	}

	mOrthographicProjectionMatrix = XMMatrixOrthographicLH(WIN_WIDTH, WIN_HEIGHT, 0.01f, 1000.0f);
	mOrthographicProjectionBuffer = new ProjectionBuffer();
	mOrthographicProjectionBuffer->SetMatrix(mOrthographicProjectionMatrix);
}


