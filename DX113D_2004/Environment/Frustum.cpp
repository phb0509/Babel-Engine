#include "Framework.h"

Frustum::Frustum() :
	mColliderRectSize(0.0f),
	mDistanceToColliderRect(1.0f),
	mbIsCheck(false),
	mCamera(nullptr),
	mbHasInitialized(false)
{
	float farz = 200.0f;
	mProjection = XMMatrixPerspectiveFovLH(XM_PIDIV4, 1.0f, 0.1f, farz);
	mCollider = new TetrahedronCollider(50.0f,farz);

}

Frustum::~Frustum()
{
}

void Frustum::Update()
{
	//mView = CAMERA->GetView(); // 타겟카메라뷰

	//Vector3 pos = CAMERA->position - CAMERA->Forward() * 1.0f;
	//Vector3 focus = pos + CAMERA->Forward();
	//view = XMMatrixLookAtLH(pos.data, focus.data, CAMERA->Up().data);
	// 위 주석처리는 프러스텀범위를 좀 더 뒤로빼서 스피어가 사라지는걸 좀 더 자연스럽게
	// 구현하기위한 위치값조정인데, 어지간하면 그냥 있는거 쓰는게 나음.

	if (mbHasInitialized)
	{
		initialize();
		mbHasInitialized = false;
	}



	Float4x4 VP;
	XMStoreFloat4x4(&VP, mView * mProjection);

	// a,b,c는 평면의 방향을 나타내는 법선벡터. d는 평면과 원점간의 거리
	//Left
	float a = VP._14 + VP._11;
	float b = VP._24 + VP._21;
	float c = VP._34 + VP._31;
	float d = VP._44 + VP._41;
	planes[0] = XMVectorSet(a, b, c, d);

	//Right
	a = VP._14 - VP._11;
	b = VP._24 - VP._21;	
	c = VP._34 - VP._31;
	d = VP._44 - VP._41;
	planes[1] = XMVectorSet(a, b, c, d);

	//Bottom
	a = VP._14 + VP._12;
	b = VP._24 + VP._22;
	c = VP._34 + VP._32;
	d = VP._44 + VP._42;
	planes[2] = XMVectorSet(a, b, c, d);

	//Top
	a = VP._14 - VP._12;
	b = VP._24 - VP._22;
	c = VP._34 - VP._32;
	d = VP._44 - VP._42;
	planes[3] = XMVectorSet(a, b, c, d);

	//Near
	a = VP._14 + VP._13;
	b = VP._24 + VP._23;
	c = VP._34 + VP._33;
	d = VP._44 + VP._43;
	planes[4] = XMVectorSet(a, b, c, d);

	//Far
	a = VP._14 - VP._13;
	b = VP._24 - VP._23;
	c = VP._34 - VP._33;
	d = VP._44 - VP._43;
	planes[5] = XMVectorSet(a, b, c, d);


	for (UINT i = 0; i < 6; i++)
		planes[i] = XMPlaneNormalize(planes[i]);

	mCollider->Update();
}

void Frustum::Render()
{
	mCollider->Render();
}

bool Frustum::ContainPoint(Vector3 position)
{
	for (UINT i = 0; i < 6; i++)
	{
		Vector3 dot = XMPlaneDotCoord(planes[i], position.data);

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
		dot = XMPlaneDotCoord(planes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//2
		edge.x = center.x + radius;
		edge.y = center.y - radius;
		edge.z = center.z - radius;
		dot = XMPlaneDotCoord(planes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//3
		edge.x = center.x + radius;
		edge.y = center.y + radius;
		edge.z = center.z - radius;
		dot = XMPlaneDotCoord(planes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//4
		edge.x = center.x - radius;
		edge.y = center.y - radius;
		edge.z = center.z + radius;
		dot = XMPlaneDotCoord(planes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//5
		edge.x = center.x + radius;
		edge.y = center.y - radius;
		edge.z = center.z + radius;
		dot = XMPlaneDotCoord(planes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//6
		edge.x = center.x - radius;
		edge.y = center.y + radius;
		edge.z = center.z + radius;
		dot = XMPlaneDotCoord(planes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//7
		edge.x = center.x - radius;
		edge.y = center.y + radius;
		edge.z = center.z - radius;
		dot = XMPlaneDotCoord(planes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//8
		edge.x = center.x + radius;
		edge.y = center.y + radius;
		edge.z = center.z + radius;
		dot = XMPlaneDotCoord(planes[i], edge.data);
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
		dot = XMPlaneDotCoord(planes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//2
		edge.x = maxBox.x;
		edge.y = minBox.y;
		edge.z = minBox.z;
		dot = XMPlaneDotCoord(planes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//3
		edge.x = minBox.x;
		edge.y = maxBox.y;
		edge.z = minBox.z;
		dot = XMPlaneDotCoord(planes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//4
		edge.x = minBox.x;
		edge.y = minBox.y;
		edge.z = maxBox.z;
		dot = XMPlaneDotCoord(planes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//5
		edge.x = maxBox.x;
		edge.y = maxBox.y;
		edge.z = minBox.z;
		dot = XMPlaneDotCoord(planes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//6
		edge.x = maxBox.x;
		edge.y = minBox.y;
		edge.z = maxBox.z;
		dot = XMPlaneDotCoord(planes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//7
		edge.x = minBox.x;
		edge.y = maxBox.y;
		edge.z = maxBox.z;
		dot = XMPlaneDotCoord(planes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		//8
		edge.x = maxBox.x;
		edge.y = maxBox.y;
		edge.z = maxBox.z;
		dot = XMPlaneDotCoord(planes[i], edge.data);
		if (dot.x > 0.0f)
			continue;

		return false;
	}

	return true;
}

void Frustum::GetPlanes(Float4* cullings)
{
	for (UINT i = 0; i < 6; i++)
		XMStoreFloat4(&cullings[i], planes[i]);
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
	mCollider->SetParent(mCamera->GetWorld());
}
