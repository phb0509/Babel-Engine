#include "Framework.h"

bool Transform::mbIsAxisDrawing = true;

Transform::Transform(string tag): 
	tag(tag),
	mPosition(0, 0, 0), 
	mRotation(0, 0, 0), 
	mScale(1, 1, 1),
	mGlobalPosition(0, 0, 0),
	mGlobalRotation(0, 0, 0),
	mGlobalScale(1, 1, 1),
	mPivot(0, 0, 0), 
	mParentMatrix(nullptr),
	isActive(false) ,
	mIsAStarPathUpdate(true),
	mMoveSpeed(10.0f),
	mRotationSpeed(10.0f)
{
	mWorldMatrix = XMMatrixIdentity();
	mWorldBuffer = new MatrixBuffer();

	mMaterial = new Material(L"Transform");
	createAxis();
	mTransformBuffer = new MatrixBuffer();

	mIsUpdateStandTimes.assign(3, true);
	mNextExecuteTimes.assign(3, -100.0f);

	mRSState = new RasterizerState();
	Collider* collider;
	
}

Transform::~Transform()
{
	delete mWorldBuffer;
	delete mTransformBuffer;
	delete mMaterial;
}

void Transform::UpdateWorld()
{
	mWorldMatrix = XMMatrixTransformation(
		mPivot.data, // ������ �߽��� �����ϴ� 3D ����.
		XMQuaternionIdentity(), // ������ ������ �����ϴ� ���ʹϾ�.
		mScale.data, // x,y,z�࿡ ���� �����ϸ��� ����.
		mPivot.data, // ȸ���߽��� �����ϴ� 3D ����.
		XMQuaternionRotationRollPitchYaw(mRotation.x, mRotation.y,mRotation.z), mPosition.data);
		// x,y,z���� ���� ��ȯ�� �����ϴ� 3D ����.
	
	if (mParentMatrix != nullptr)
		mWorldMatrix *= *mParentMatrix;

	XMMatrixDecompose(&mGlobalScale.data, &mGlobalRotation.data,
		&mGlobalPosition.data, mWorldMatrix);

	mWorldBuffer->Set(mWorldMatrix); // Matrix���� ��ġ��ķ� �ٲ㼭 MatrixBuffer�� Set.
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




void Transform::createAxis()
{
	float length = 3.0f;
	float thickness = 0.5f;

	//Axis X		
	Float4 color = { 1, 0, 0, 1 };
	mVertices.emplace_back(Float3(0, 0, 0), color);
	mVertices.emplace_back(Float3(length, 0, 0), color);
	mVertices.emplace_back(Float3(length, thickness, 0), color);
	mVertices.emplace_back(Float3(length, 0, thickness), color);

	mIndices.emplace_back(0);
	mIndices.emplace_back(2);
	mIndices.emplace_back(1);

	mIndices.emplace_back(0);
	mIndices.emplace_back(1);
	mIndices.emplace_back(3);

	mIndices.emplace_back(0);
	mIndices.emplace_back(3);
	mIndices.emplace_back(2);

	mIndices.emplace_back(1);
	mIndices.emplace_back(2);
	mIndices.emplace_back(3);

	//Axis Y
	color = { 0, 1, 0, 1 };
	mVertices.emplace_back(Float3(0, 0, 0), color);
	mVertices.emplace_back(Float3(0, length, 0), color);
	mVertices.emplace_back(Float3(0, length, thickness), color);
	mVertices.emplace_back(Float3(thickness, length, 0), color);

	mIndices.emplace_back(4);
	mIndices.emplace_back(6);
	mIndices.emplace_back(5);

	mIndices.emplace_back(4);
	mIndices.emplace_back(5);
	mIndices.emplace_back(7);

	mIndices.emplace_back(4);
	mIndices.emplace_back(7);
	mIndices.emplace_back(6);

	mIndices.emplace_back(5);
	mIndices.emplace_back(6);
	mIndices.emplace_back(7);

	//Axis Z
	color = { 0, 0, 1, 1 };
	mVertices.emplace_back(Float3(0, 0, 0), color);
	mVertices.emplace_back(Float3(0, 0, length), color);
	mVertices.emplace_back(Float3(thickness, 0, length), color);
	mVertices.emplace_back(Float3(0, thickness, length), color);

	mIndices.emplace_back(8);
	mIndices.emplace_back(10);
	mIndices.emplace_back(9);

	mIndices.emplace_back(8);
	mIndices.emplace_back(9);
	mIndices.emplace_back(11);

	mIndices.emplace_back(8);
	mIndices.emplace_back(11);
	mIndices.emplace_back(10);

	mIndices.emplace_back(9);
	mIndices.emplace_back(10);
	mIndices.emplace_back(11);

	mMesh = new Mesh(mVertices.data(), sizeof(VertexColor), mVertices.size(),
		mIndices.data(), mIndices.size());
}

void Transform::RenderAxis()
{
	if (!mbIsAxisDrawing)
		return;

	Vector3 scale(1, 1, 1);
	Matrix matrix = XMMatrixTransformation(mPivot.data, XMQuaternionIdentity(),
		scale.data, mPivot.data, mGlobalRotation.data, mGlobalPosition.data);

	mTransformBuffer->Set(matrix);
	mTransformBuffer->SetVSBuffer(0);

	mMesh->IASet();
	mMaterial->Set();

	mRSState->FillMode(D3D11_FILL_WIREFRAME);
	mRSState->SetState();
	DEVICECONTEXT->DrawIndexed(mIndices.size(), 0, 0);
}