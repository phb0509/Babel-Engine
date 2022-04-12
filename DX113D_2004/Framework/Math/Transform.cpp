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
		mPivot.data, // ������ �߽��� �����ϴ� 3D ����.
		XMQuaternionIdentity(), // ������ ������ �����ϴ� ���ʹϾ�.
		mScale.data, // x,y,z�࿡ ���� �����ϸ��� ����.
		mPivot.data, // ȸ���߽��� �����ϴ� 3D ����.
		XMQuaternionRotationRollPitchYaw(mRotation.x, mRotation.y, mRotation.z), 
		mPosition.data
	);
	// x,y,z���� ���� ��ȯ�� �����ϴ� 3D ����.

	mLocalWorldMatrix = mWorldMatrix;

	if (mParentMatrix != nullptr)
	{
		mWorldMatrix *= *mParentMatrix;
	}
		
	XMMatrixDecompose(&mGlobalScale.data, &mGlobalRotation.data,
		&mGlobalPosition.data, mWorldMatrix);

	mWorldBuffer->SetMatrix(mWorldMatrix); // Matrix���� ��ġ��ķ� �ٲ㼭 MatrixBuffer�� Set.
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

void Transform::RotateToDestinationForModel(Transform* transform, Vector3 dest) // ȸ����Ű���� �ϴ� transform�� ��ǥ��������. 
{
	dest = dest - transform->mPosition;
	dest.Normalize();
	Vector3 forward = transform->Forward() * -1.0f; // �� ������ �Ųٷ� �Ǿ��־ -1 ��
	float temp = Vector3::Dot(forward, dest); // �󸶳� ȸ������.
	temp = acos(temp);

	Vector3 tempDirection = Vector3::Cross(forward, dest); // ���� ȸ������.

	if (tempDirection.y < 0.0f) // ��ǥ�����𷺼��� �����庤�ͺ��� ���ʿ� ������
	{
		transform->mRotation.y -= temp;
	}

	else if (tempDirection.y >= 0.0f) // �𷺼��� �����庤�ͺ��� �����ʿ� ������
	{
		transform->mRotation.y += temp;
	}
}

void Transform::RotateToDestinationForNotModel(Transform* transform, Vector3 dest)
{
	dest = dest - transform->mPosition;
	dest.Normalize();
	Vector3 forward = transform->Forward();
	float temp = Vector3::Dot(forward, dest); // �󸶳� ȸ������.
	temp = acos(temp);

	Vector3 tempDirection = Vector3::Cross(forward, dest); // ���� ȸ������.

	if (tempDirection.y < 0.0f) // ��ǥ�����𷺼��� �����庤�ͺ��� ���ʿ� ������
	{
		transform->mRotation.y -= temp;
	}

	else if (tempDirection.y >= 0.0f) // �𷺼��� �����庤�ͺ��� �����ʿ� ������
	{
		transform->mRotation.y += temp;
	}
}

void Transform::MoveToDestination(Transform* transform, Vector3 dest, float moveSpeed) // �ܼ� dest���� position+=
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

	if (mIsUpdateStandTimes[1]) // �ʱ⿡�� true
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

	if (Timer::Get()->GetRunTime() >= mNextExecuteTimes[2]) // �ð� ������
	{
		mIsUpdateStandTimes[2] = true;
	}

	return mIsUpdateStandTimes[2];
}


void Transform::SetHashColorBuffer()
{
	mHashColorBuffer->SetMatrix(mHashColor); // ������ ����Ÿ�ٿ� �׸� ������Ʈ�� �ؽ��÷�.
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






