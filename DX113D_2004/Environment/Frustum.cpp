#include "Framework.h"

Frustum::Frustum() :
	mColliderRectSize(0.0f),
	mDistanceToColliderRect(1.0f),
	mbIsCheck(false),
	mCamera(nullptr),
	mbHasInitialized(false),
	mDistanceToNearZ(10.0f),
	mDistanceToFarZ(200.0f),
	mAspectRatio(WIN_WIDTH / (float)WIN_HEIGHT),
	mFoV(XM_PIDIV4)
{
	mProjection = XMMatrixPerspectiveFovLH(mFoV, mAspectRatio, mDistanceToNearZ, mDistanceToFarZ);

	createFrustumCollider();

	mEmptyObject = new EmptyObject();
}

Frustum::~Frustum()
{
}

void Frustum::Update()
{
	// Vector3 pos = CAMERA->position - CAMERA->Forward() * 1.0f;
	// Vector3 focus = pos + CAMERA->Forward();
	// view = XMMatrixLookAtLH(pos.data, focus.data, CAMERA->Up().data);
	// 위 주석처리는 프러스텀범위를 좀 더 뒤로빼서 스피어가 사라지는걸 좀 더 자연스럽게
	// 구현하기위한 위치값조정인데, 어지간하면 그냥 있는거 쓰는게 나음.

	mView = CAMERA->GetView(); // 타겟카메라뷰

	if (!mbHasInitialized)
	{
		initialize();
		mbHasInitialized = true;
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


	mEmptyObject->Update();
	mCollider->Update();
	//moveFrustumCollider();
}

void Frustum::Render()
{
	mEmptyObject->Render();
	mCollider->Render();
}

void Frustum::PostRender()
{
	/*ImGui::Begin("Test");

	string c1 = "CubePosition";
	string c2 = "CubeRotation";
	string c3 = "CubeScale";
	ImGui::InputFloat3(c1.c_str(), (float*)&mEmptyObject->mPosition);
	ImGui::InputFloat3(c2.c_str(), (float*)&mEmptyObject->mRotation);
	ImGui::InputFloat3(c3.c_str(), (float*)&mEmptyObject->mScale);

	string fc1 = "ColliderPosition";
	string fc2 = "ColliderRotation";
	string fc3 = "ColliderScale";
	ImGui::InputFloat3(fc1.c_str(), (float*)&mCollider->mPosition);
	ImGui::InputFloat3(fc2.c_str(), (float*)&mCollider->mRotation);
	ImGui::InputFloat3(fc3.c_str(), (float*)&mCollider->mScale);

	ImGui::End();*/
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

void Frustum::moveFrustumCollider()
{


}


void Frustum::initialize()
{
	//mEmptyObject->SetParent(GM->GetPlayer()->GetWorld());
	float tempScale = mCamera->GetCameraTarget()->mScale.x; // 플레이어 스케일값.
	tempScale = 1.0f / tempScale;

	mEmptyObject->SetParent(GM->GetPlayer()->GetWorld());

	mCollider->SetParent(mEmptyObject->GetWorld());
	mCollider->mScale = { tempScale,tempScale,tempScale };// 플레이어스케일 줄인만큼 자식에서 늘려줘야함.
	mCollider->mRotation.y += 3.141592f; // 반대로되어있어서 180도 돌려줘야함.

	mEmptyObject->mPosition.z += mCamera->GetDistanceToTarget();
}

void Frustum::createFrustumCollider()
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
		XMStoreFloat4(&cullings[i], planes[i]); // 절두체 면을 cullings에 저장.
}