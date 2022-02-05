#include "Framework.h"

bool Transform::mbIsRenderGizmos = true;

Transform::Transform(string mTag) :
	mTag(mTag),
	mPosition(0, 0, 0),
	mRotation(0, 0, 0),
	mScale(1, 1, 1),
	mGlobalPosition(0, 0, 0),
	mGlobalRotation(0, 0, 0),
	mGlobalScale(1, 1, 1),
	mPivot(0, 0, 0),
	mParentMatrix(nullptr),
	mbIsActive(false),
	mIsAStarPathUpdate(true),
	mMoveSpeed(10.0f),
	mRotationSpeed(10.0f)
{
	createHashColor();
	mWorldMatrix = XMMatrixIdentity();
	mWorldBuffer = new MatrixBuffer();
	mColorBuffer = new ColorBuffer();

	mGizmosMaterial = new Material(L"Gizmos");

	createGizmoseHashColor();
	createGizmos();
	mGizmosWorldBuffer = new MatrixBuffer();

	mIsUpdateStandTimes.assign(3, true);
	mNextExecuteTimes.assign(3, -100.0f);

	mRSState = new RasterizerState();
}

Transform::~Transform()
{
	delete mWorldBuffer;
	delete mGizmosWorldBuffer;
	delete mGizmosMaterial;
}

void Transform::UpdateWorld()
{
	mWorldMatrix = XMMatrixTransformation(
		mPivot.data, // 배율의 중심을 설명하는 3D 벡터.
		XMQuaternionIdentity(), // 배율의 방향을 설명하는 쿼터니언.
		mScale.data, // x,y,z축에 대한 스케일링값 벡터.
		mPivot.data, // 회전중심을 설명하는 3D 벡터.
		XMQuaternionRotationRollPitchYaw(mRotation.x, mRotation.y, mRotation.z), mPosition.data);
	// x,y,z축을 따라 변환을 설명하는 3D 벡터.

	if (mParentMatrix != nullptr)
		mWorldMatrix *= *mParentMatrix;

	XMMatrixDecompose(&mGlobalScale.data, &mGlobalRotation.data,
		&mGlobalPosition.data, mWorldMatrix);

	mWorldBuffer->Set(mWorldMatrix); // Matrix값을 전치행렬로 바꿔서 MatrixBuffer에 Set.
}

void Transform::PreRenderGizmosForColorPicking()
{
	mGizmosMaterial->SetShader(L"GizmosColorPicking");

	Vector3 tempPosition = { 400.0f,0.0f,0.0f };
	Vector3 scale(30, 30, 30);

	Matrix worldMatrix = XMMatrixTransformation(
		mPivot.data,
		XMQuaternionIdentity(),
		scale.data,
		mPivot.data,
		mGlobalRotation.data,
		//tempPosition.data,
		mPosition.data
	);

	mGizmosWorldBuffer->Set(worldMatrix); // Set WorldBuffer
	mGizmosWorldBuffer->SetVSBuffer(0);

	mGizmosMesh->IASet();
	mGizmosMaterial->Set();

	//Environment::Get()->SetOrthographicProjectionBuffer(); // Set ProjectionBuffer
	DEVICECONTEXT->DrawIndexed(mGizmosIndices.size(), 0, 0); // Draw Gizmos

	Environment::Get()->SetPerspectiveProjectionBuffer(); // Perspective로 다시 돌려놓기.
}

void Transform::RenderGizmos()
{
	mGizmosMaterial->SetShader(L"Gizmos");

	Vector3 tempPosition = { 400.0f,0.0f,0.0f };
	Vector3 scale(30, 30, 30);

	Matrix worldMatrix =
		XMMatrixTransformation(
			mPivot.data,
			XMQuaternionIdentity(),
			scale.data,
			mPivot.data,
			mGlobalRotation.data,
			//tempPosition.data,
			mPosition.data
		);

	mGizmosWorldBuffer->Set(worldMatrix);
	mGizmosWorldBuffer->SetVSBuffer(0);

	mGizmosMesh->IASet();
	mGizmosMaterial->Set();

	//mRSState->FillMode(D3D11_FILL_WIREFRAME);
	mRSState->FillMode(D3D11_FILL_SOLID);
	mRSState->SetState();

	//Environment::Get()->SetOrthographicProjectionBuffer();
	DEVICECONTEXT->DrawIndexed(mGizmosIndices.size(), 0, 0);

	Environment::Get()->SetPerspectiveProjectionBuffer();
}

void Transform::SetWorldBuffer(UINT slot)
{
	mWorldBuffer->SetVSBuffer(slot);
}

Vector3 Transform::Forward()
{
	return XMVector3Normalize(XMVector3TransformNormal(kForward, mWorldMatrix));
}

Vector3 Transform::Up()
{
	return XMVector3Normalize(XMVector3TransformNormal(kUp, mWorldMatrix));
}

Vector3 Transform::Right()
{
	return XMVector3Normalize(XMVector3TransformNormal(kRight, mWorldMatrix));
}

void Transform::RotateToDestinationForModel(Transform* transform, Vector3 dest) // 회전시키고자 하는 transform과 목표지점벡터. 
{
	dest = dest - transform->mPosition;
	dest.Normalize();
	Vector3 forward = transform->Forward() * -1.0f; // 모델 포워드 거꾸로 되어있어서 -1 곱
	float temp = Vector3::Dot(forward, dest); // 얼마나 회전할지.
	temp = acos(temp);

	Vector3 tempDirection = Vector3::Cross(forward, dest); // 어디로 회전할지.

	if (tempDirection.y < 0.0f) // 목표지점디렉션이 포워드벡터보다 왼쪽에 있으면
	{
		transform->mRotation.y -= temp;
	}

	else if (tempDirection.y >= 0.0f) // 디렉션이 포워드벡터보다 오른쪽에 있으면
	{
		transform->mRotation.y += temp;
	}
}

void Transform::RotateToDestinationForNotModel(Transform* transform, Vector3 dest)
{
	dest = dest - transform->mPosition;
	dest.Normalize();
	Vector3 forward = transform->Forward();
	float temp = Vector3::Dot(forward, dest); // 얼마나 회전할지.
	temp = acos(temp);

	Vector3 tempDirection = Vector3::Cross(forward, dest); // 어디로 회전할지.

	if (tempDirection.y < 0.0f) // 목표지점디렉션이 포워드벡터보다 왼쪽에 있으면
	{
		transform->mRotation.y -= temp;
	}

	else if (tempDirection.y >= 0.0f) // 디렉션이 포워드벡터보다 오른쪽에 있으면
	{
		transform->mRotation.y += temp;
	}
}

void Transform::MoveToDestination(Transform* transform, Vector3 dest, float moveSpeed) // 단순 dest까지 position+=
{
	Vector3 mDirection = (dest - transform->mPosition).Normal();

	transform->mPosition.x += mDirection.x * moveSpeed * DELTA;
	transform->mPosition.z += mDirection.z * moveSpeed * DELTA;
}

void Transform::ExecuteRotationPeriodFunction(function<void(Transform*, Vector3)> funcPointer, Transform* param1, Vector3 param2, float periodTime)
{
	if (mIsUpdateStandTimes[0])
	{
		mNextExecuteTimes[0] = Timer::Get()->GetRunTime() + periodTime;
		mIsUpdateStandTimes[0] = false;
	}

	if (Timer::Get()->GetRunTime() >= mNextExecuteTimes[0])
	{
		funcPointer(param1, param2);
		mIsUpdateStandTimes[0] = true;
	}
}

void Transform::ExecuteAStarUpdateFunction(function<void(Vector3)> funcPointer, Vector3 param1, float periodTime)
{
	if (Timer::Get()->GetRunTime() >= mNextExecuteTimes[1])
	{
		funcPointer(param1);

		mIsUpdateStandTimes[1] = true;
	}

	if (mIsUpdateStandTimes[1]) // 초기에는 true
	{
		mNextExecuteTimes[1] = Timer::Get()->GetRunTime() + periodTime;
		mIsUpdateStandTimes[1] = false;
	}
}

bool Transform::CheckTime(float periodTime)
{
	if (mIsUpdateStandTimes[2])
	{
		mNextExecuteTimes[2] = Timer::Get()->GetRunTime() + periodTime;
		mIsUpdateStandTimes[2] = false;
	}

	if (Timer::Get()->GetRunTime() >= mNextExecuteTimes[2]) // 시간 지나면
	{
		mIsUpdateStandTimes[2] = true;
	}

	return mIsUpdateStandTimes[2];
}


void Transform::SetColorBuffer()
{
	mColorBuffer->Set(mHashColorForBuffer);
	mColorBuffer->SetVSBuffer(10);
}

void Transform::createHashColor()
{
	int hashValue = rand() % 1000000;

	int a = (hashValue >> 24) & 0xff;

	int b = (hashValue >> 16) & 0xff;

	int g = (hashValue >> 8) & 0xff;

	int r = hashValue & 0xff;

	float fr = static_cast<float>(r);
	float fg = static_cast<float>(g);
	float fb = static_cast<float>(b);

	Float4 tempColor = { fr / 255.0f,fg / 255.0f,fb / 255.0f,1.0f };

	mHashColorForBuffer = tempColor;
}

void Transform::createGizmos()
{
	float length = 3.0f;
	float thickness = 0.5f;

	Collider* collider;

	//Axis X		

	Float4 color = { 1, 0, 0, 1 };
	mGizmosVertices.emplace_back(Float3(0, 0, 0), color, mGizmosHashColor.x);
	mGizmosVertices.emplace_back(Float3(length, 0, 0), color, mGizmosHashColor.x);
	mGizmosVertices.emplace_back(Float3(length, thickness, 0), color, mGizmosHashColor.x);
	mGizmosVertices.emplace_back(Float3(length, 0, thickness), color, mGizmosHashColor.x);

	mGizmosIndices.emplace_back(0);
	mGizmosIndices.emplace_back(2);
	mGizmosIndices.emplace_back(1);

	mGizmosIndices.emplace_back(0);
	mGizmosIndices.emplace_back(1);
	mGizmosIndices.emplace_back(3);

	mGizmosIndices.emplace_back(0);
	mGizmosIndices.emplace_back(3);
	mGizmosIndices.emplace_back(2);

	mGizmosIndices.emplace_back(1);
	mGizmosIndices.emplace_back(2);
	mGizmosIndices.emplace_back(3);

	//Axis Y
	color = { 0, 1, 0, 1 };
	mGizmosVertices.emplace_back(Float3(0, 0, 0), color, mGizmosHashColor.y);
	mGizmosVertices.emplace_back(Float3(0, length, 0), color, mGizmosHashColor.y);
	mGizmosVertices.emplace_back(Float3(0, length, thickness), color, mGizmosHashColor.y);
	mGizmosVertices.emplace_back(Float3(thickness, length, 0), color, mGizmosHashColor.y);

	mGizmosIndices.emplace_back(4);
	mGizmosIndices.emplace_back(6);
	mGizmosIndices.emplace_back(5);

	mGizmosIndices.emplace_back(4);
	mGizmosIndices.emplace_back(5);
	mGizmosIndices.emplace_back(7);

	mGizmosIndices.emplace_back(4);
	mGizmosIndices.emplace_back(7);
	mGizmosIndices.emplace_back(6);

	mGizmosIndices.emplace_back(5);
	mGizmosIndices.emplace_back(6);
	mGizmosIndices.emplace_back(7);

	//Axis Z
	color = { 0, 0, 1, 1 };
	mGizmosVertices.emplace_back(Float3(0, 0, 0), color, mGizmosHashColor.z);
	mGizmosVertices.emplace_back(Float3(0, 0, length), color, mGizmosHashColor.z);
	mGizmosVertices.emplace_back(Float3(thickness, 0, length), color, mGizmosHashColor.z);
	mGizmosVertices.emplace_back(Float3(0, thickness, length), color, mGizmosHashColor.z);

	mGizmosIndices.emplace_back(8);
	mGizmosIndices.emplace_back(10);
	mGizmosIndices.emplace_back(9);

	mGizmosIndices.emplace_back(8);
	mGizmosIndices.emplace_back(9);
	mGizmosIndices.emplace_back(11);

	mGizmosIndices.emplace_back(8);
	mGizmosIndices.emplace_back(11);
	mGizmosIndices.emplace_back(10);

	mGizmosIndices.emplace_back(9);
	mGizmosIndices.emplace_back(10);
	mGizmosIndices.emplace_back(11);

	mGizmosMesh = new Mesh(mGizmosVertices.data(), sizeof(VertexColor), mGizmosVertices.size(),
		mGizmosIndices.data(), mGizmosIndices.size());
}

void Transform::createGizmoseHashColor()
{
	int hashValueX = rand() % 1000000;
	int a = (hashValueX >> 24) & 0xff;
	int b = (hashValueX >> 16) & 0xff;
	int g = (hashValueX >> 8) & 0xff;
	int r = hashValueX & 0xff;

	float fr = static_cast<float>(r);
	float fg = static_cast<float>(g);
	float fb = static_cast<float>(b);

	Float4 tempColor = { fr / 255.0f,fg / 255.0f,fb / 255.0f,1.0f };

	mGizmosHashColor.x = tempColor;

	int hashValueY = rand() % 1000000;
	a = (hashValueY >> 24) & 0xff;
	b = (hashValueY >> 16) & 0xff;
	g = (hashValueY >> 8) & 0xff;
	r = hashValueY & 0xff;

	fr = static_cast<float>(r);
	fg = static_cast<float>(g);
	fb = static_cast<float>(b);

	tempColor = { fr / 255.0f,fg / 255.0f,fb / 255.0f,1.0f };

	mGizmosHashColor.y = tempColor;

	int hashValueZ = rand() % 1000000;
	a = (hashValueZ >> 24) & 0xff;
	b = (hashValueZ >> 16) & 0xff;
	g = (hashValueZ >> 8) & 0xff;
	r = hashValueZ & 0xff;

	fr = static_cast<float>(r);
	fg = static_cast<float>(g);
	fb = static_cast<float>(b);

	tempColor = { fr / 255.0f,fg / 255.0f,fb / 255.0f,1.0f };

	mGizmosHashColor.z = tempColor;
}


