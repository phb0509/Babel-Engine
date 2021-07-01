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
	// �� �ּ�ó���� �������ҹ����� �� �� �ڷλ��� ���Ǿ ������°� �� �� �ڿ�������
	// �����ϱ����� ��ġ�������ε�, �������ϸ� �׳� �ִ°� ���°� ����.

	mView = CAMERA->GetView(); // Ÿ��ī�޶��

	if (!mbHasInitialized)
	{
		initialize();
		mbHasInitialized = true;
	}

	Float4x4 VP;
	XMStoreFloat4x4(&VP, mView * mProjection);

	// a,b,c�� ����� ������ ��Ÿ���� ��������. d�� ���� �������� �Ÿ�

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
	float tempScale = mCamera->GetCameraTarget()->mScale.x; // �÷��̾� �����ϰ�.
	tempScale = 1.0f / tempScale;

	mEmptyObject->SetParent(GM->GetPlayer()->GetWorld());

	mCollider->SetParent(mEmptyObject->GetWorld());
	mCollider->mScale = { tempScale,tempScale,tempScale };// �÷��̾���� ���θ�ŭ �ڽĿ��� �÷������.
	mCollider->mRotation.y += 3.141592f; // �ݴ�εǾ��־ 180�� ���������.

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
		XMStoreFloat4(&cullings[i], planes[i]); // ����ü ���� cullings�� ����.
}