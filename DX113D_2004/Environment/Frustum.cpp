#include "Framework.h"

Frustum::Frustum(float FoV, float aspectRatio, float distanceToNearZ, float distanceToFarZ) :
	mColliderRectSize(0.0f),
	mDistanceToColliderRect(1.0f),
	mbIsCheck(false),
	mCamera(nullptr),
	mbHasInitialized(false),
	mFoV(FoV),
	mAspectRatio(aspectRatio),
	mDistanceToNearZ(distanceToNearZ),
	mDistanceToFarZ(distanceToFarZ),
	mbIsTerrainFrustumCulling(false)
{
	mProjectionMatrix = XMMatrixPerspectiveFovLH(mFoV, mAspectRatio, mDistanceToNearZ, mDistanceToFarZ);

	createCollider();

	mEmptyObject = new EmptyObject();
}

Frustum::~Frustum()
{
	GM->SafeDelete(mCollider);
	GM->SafeDelete(mEmptyObject);
}

void Frustum::Update()
{
	// Vector3 pos = CAMERA->position - CAMERA->Forward() * 1.0f;
	// Vector3 focus = pos + CAMERA->Forward();
	// view = XMMatrixLookAtLH(pos.data, focus.data, CAMERA->Up().data);
	// �� �ּ�ó���� �������ҹ����� �� �� �ڷλ��� ���Ǿ ������°� �� �� �ڿ�������
	// �����ϱ����� ��ġ�������ε�, �������ϸ� �׳� �ִ°� ���°� ����.

	if (!mbHasInitialized)
	{
		initialize(); // Ư�� ���� �������� �ö��̴�������.
		mbHasInitialized = true;
	}

	mProjectionMatrix = mCamera->GetProjectionMatrixInUse();
	Float4x4 VP;
	XMStoreFloat4x4(&VP, mView * mProjectionMatrix);

	// a,b,c�� ����� ������ ��Ÿ���� ��������. d�� ���� �������� �Ÿ�

	//Left
	float a = VP._14 + VP._11;
	float b = VP._24 + VP._21;
	float c = VP._34 + VP._31;
	float d = VP._44 + VP._41;
	mPlanes[0] = XMVectorSet(a, b, c, d);

	//Right
	a = VP._14 - VP._11;
	b = VP._24 - VP._21;
	c = VP._34 - VP._31;
	d = VP._44 - VP._41;
	mPlanes[1] = XMVectorSet(a, b, c, d);

	//Bottom
	a = VP._14 + VP._12;
	b = VP._24 + VP._22;
	c = VP._34 + VP._32;
	d = VP._44 + VP._42;
	mPlanes[2] = XMVectorSet(a, b, c, d);

	//Top
	a = VP._14 - VP._12;
	b = VP._24 - VP._22;
	c = VP._34 - VP._32;
	d = VP._44 - VP._42;
	mPlanes[3] = XMVectorSet(a, b, c, d);

	//Near
	a = VP._14 + VP._13;
	b = VP._24 + VP._23;
	c = VP._34 + VP._33;
	d = VP._44 + VP._43;
	mPlanes[4] = XMVectorSet(a, b, c, d);

	//Far
	a = VP._14 - VP._13;
	b = VP._24 - VP._23;
	c = VP._34 - VP._33;
	d = VP._44 - VP._43;
	mPlanes[5] = XMVectorSet(a, b, c, d);


	for (UINT i = 0; i < 6; i++)
	{
		mPlanes[i] = XMPlaneNormalize(mPlanes[i]);
	}
		
	mEmptyObject->Update();
	mCollider->Update();

}

void Frustum::RenderCollider()
{
	mEmptyObject->Render();
	mCollider->Render();
}

void Frustum::PostRender()
{

}

void Frustum::setCollider(float colliderRectSize, float distanceToColliderRect)
{
	if (!mbIsCheck)
	{
		mColliderRectSize = colliderRectSize;
		mDistanceToColliderRect = distanceToColliderRect;
		mbIsCheck = true;
	}
}

void Frustum::initialize()
{
	float tempScale = GM->GetPlayer()->mScale.x;
	tempScale = 1.0f / tempScale;

	mEmptyObject->SetParent(GM->GetPlayer()->GetWorldMatrix());

	mCollider->SetParent(mEmptyObject->GetWorldMatrix());
	mCollider->mScale = { tempScale,tempScale,tempScale };// �÷��̾���� ���θ�ŭ �ڽĿ��� �÷������.
	mCollider->mRotation.y += 3.141592f; // �ݴ�εǾ��־ 180�� ���������.

	mEmptyObject->mPosition.z += 13.0f;
}

void Frustum::createCollider()
{
	float nearHeight = 2 * tan(mFoV / 2.0f) * mDistanceToNearZ;
	float nearWidth = nearHeight * mAspectRatio;

	float farHeight = 2 * tan(mFoV / 2.0f) * mDistanceToFarZ;
	float farWidth = farHeight * mAspectRatio;

	mCollider = new FrustumCollider(nearWidth,nearHeight,farWidth,farHeight,mDistanceToNearZ,mDistanceToFarZ);
}

bool Frustum::ContainPoint(Vector3 position)
{
	for (UINT i = 0; i < 6; i++)
	{
		Vector3 dot = XMPlaneDotCoord(mPlanes[i], position.data);

		if (dot.x < 0.0f)
			return false;
	}

	return true;
}

bool Frustum::ContainSphere(Vector3 center, float radius)
{
	Vector3 edge;
	Vector3 dot;

	for (UINT i = 0; i < 6; i++)
	{
		//1
		edge.x = center.x - radius;
		edge.y = center.y - radius;
		edge.z = center.z - radius;
		dot = XMPlaneDotCoord(mPlanes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//2
		edge.x = center.x + radius;
		edge.y = center.y - radius;
		edge.z = center.z - radius;
		dot = XMPlaneDotCoord(mPlanes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//3
		edge.x = center.x + radius;
		edge.y = center.y + radius;
		edge.z = center.z - radius;
		dot = XMPlaneDotCoord(mPlanes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//4
		edge.x = center.x - radius;
		edge.y = center.y - radius;
		edge.z = center.z + radius;
		dot = XMPlaneDotCoord(mPlanes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//5
		edge.x = center.x + radius;
		edge.y = center.y - radius;
		edge.z = center.z + radius;
		dot = XMPlaneDotCoord(mPlanes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//6
		edge.x = center.x - radius;
		edge.y = center.y + radius;
		edge.z = center.z + radius;
		dot = XMPlaneDotCoord(mPlanes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//7
		edge.x = center.x - radius;
		edge.y = center.y + radius;
		edge.z = center.z - radius;
		dot = XMPlaneDotCoord(mPlanes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//8
		edge.x = center.x + radius;
		edge.y = center.y + radius;
		edge.z = center.z + radius;
		dot = XMPlaneDotCoord(mPlanes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		return false;
	}

	return true;
}

bool Frustum::ContainBox(Vector3 minBox, Vector3 maxBox)
{
	Vector3 edge;
	Vector3 dot;

	for (UINT i = 0; i < 6; i++)
	{
		//1
		edge.x = minBox.x;
		edge.y = minBox.y;
		edge.z = minBox.z;
		dot = XMPlaneDotCoord(mPlanes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//2
		edge.x = maxBox.x;
		edge.y = minBox.y;
		edge.z = minBox.z;
		dot = XMPlaneDotCoord(mPlanes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//3
		edge.x = minBox.x;
		edge.y = maxBox.y;
		edge.z = minBox.z;
		dot = XMPlaneDotCoord(mPlanes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//4
		edge.x = minBox.x;
		edge.y = minBox.y;
		edge.z = maxBox.z;
		dot = XMPlaneDotCoord(mPlanes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//5
		edge.x = maxBox.x;
		edge.y = maxBox.y;
		edge.z = minBox.z;
		dot = XMPlaneDotCoord(mPlanes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//6
		edge.x = maxBox.x;
		edge.y = minBox.y;
		edge.z = maxBox.z;
		dot = XMPlaneDotCoord(mPlanes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//7
		edge.x = minBox.x;
		edge.y = maxBox.y;
		edge.z = maxBox.z;
		dot = XMPlaneDotCoord(mPlanes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//8
		edge.x = maxBox.x;
		edge.y = maxBox.y;
		edge.z = maxBox.z;
		dot = XMPlaneDotCoord(mPlanes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		return false;
	}

	return true;
}

void Frustum::GetPlanes(Float4* cullings)
{
	for (UINT i = 0; i < 6; i++)
	{
		XMStoreFloat4(&cullings[i], mPlanes[i]); // ����ü �� ���� cullings�� ����.
	}
}

void Frustum::GetPlanesForTerrainFrustumCulling(Float4* cullings)
{
	Matrix projectionMatrix = XMMatrixPerspectiveFovLH(mCamera->GetFoV(), 1.0f, mCamera->GetDistanceToNearZ(), mCamera->GetDistanceToFarZ());

	Float4x4 VP;
	XMStoreFloat4x4(&VP, mView * projectionMatrix);

	//Left
	float a = VP._14 + VP._11;
	float b = VP._24 + VP._21;
	float c = VP._34 + VP._31;
	float d = VP._44 + VP._41;
	mPlanes[0] = XMVectorSet(a, b, c, d);

	//Right
	a = VP._14 - VP._11;
	b = VP._24 - VP._21;
	c = VP._34 - VP._31;
	d = VP._44 - VP._41;
	mPlanes[1] = XMVectorSet(a, b, c, d);

	//Bottom
	a = VP._14 + VP._12;
	b = VP._24 + VP._22;
	c = VP._34 + VP._32;
	d = VP._44 + VP._42;
	mPlanes[2] = XMVectorSet(a, b, c, d);

	//Top
	a = VP._14 - VP._12;
	b = VP._24 - VP._22;
	c = VP._34 - VP._32;
	d = VP._44 - VP._42;
	mPlanes[3] = XMVectorSet(a, b, c, d);

	//Near
	a = VP._14 + VP._13;
	b = VP._24 + VP._23;
	c = VP._34 + VP._33;
	d = VP._44 + VP._43;
	mPlanes[4] = XMVectorSet(a, b, c, d);

	//Far
	a = VP._14 - VP._13;
	b = VP._24 - VP._23;
	c = VP._34 - VP._33;
	d = VP._44 - VP._43;
	mPlanes[5] = XMVectorSet(a, b, c, d);


	for (UINT i = 0; i < 6; i++)
	{
		mPlanes[i] = XMPlaneNormalize(mPlanes[i]);
	}

	for (UINT i = 0; i < 6; i++)
	{
		XMStoreFloat4(&cullings[i], mPlanes[i]); // ����ü �� ���� cullings�� ����.
	}
}
