#include "Framework.h"



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
	mHashColorBuffer = new ColorBuffer();

	mMaterial = new Material(L"Gizmos");
	mIsUpdateStandTimes.assign(3, true);
	mNextExecuteTimes.assign(3, -100.0f);

	mRSState = new RasterizerState();
}

Transform::~Transform()
{
	delete mWorldBuffer;
	delete mMaterial;
}

void Transform::UpdateWorld()
{
	mWorldMatrix = XMMatrixTransformation(
		mPivot.data, // 배율의 중심을 설명하는 3D 벡터.
		XMQuaternionIdentity(), // 배율의 방향을 설명하는 쿼터니언.
		mScale.data, // x,y,z축에 대한 스케일링값 벡터.
		mPivot.data, // 회전중심을 설명하는 3D 벡터.
		XMQuaternionRotationRollPitchYaw(mRotation.x, mRotation.y, mRotation.z), 
		mPosition.data
	);
	// x,y,z축을 따라 변환을 설명하는 3D 벡터.

	mLocalWorldMatrix = mWorldMatrix;

	if (mParentMatrix != nullptr)
	{
		mWorldMatrix *= *mParentMatrix;
	}
		
	XMMatrixDecompose(&mGlobalScale.data, &mGlobalRotation.data,
		&mGlobalPosition.data, mWorldMatrix);

	mWorldBuffer->SetMatrix(mWorldMatrix); // Matrix값을 전치행렬로 바꿔서 MatrixBuffer에 Set.
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


void Transform::SetHashColorBuffer()
{
	mHashColorBuffer->SetMatrix(mHashColor); // 별도의 렌더타겟에 그릴 오브젝트의 해쉬컬러.
	mHashColorBuffer->SetVSBuffer(10);
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

	mHashColor = tempColor;
}






