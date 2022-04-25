#pragma once

class Monster;
class Collider;
class BoxCollider;
class SphereCollider;
class CapsuleCollider;

class Transform
{
	struct GizmosHashColor
	{
		Float4 x;
		Float4 y;
		Float4 z;
	};

public:
	Transform(string mTag = "Untagged");
	virtual ~Transform();

	void UpdateWorld();
	void SetWorldBuffer(UINT slot = 0);

	Matrix* GetWorldMatrix() { return &mWorldMatrix; }
	Matrix GetWorldMatrixValue() { return mWorldMatrix; }
	Matrix GetInverseWorldMatrix() { return XMMatrixInverse(nullptr, mWorldMatrix); }
	Matrix GetLocalWorldMatrix() { return mLocalWorldMatrix; }
	Matrix* GetParentMatrix() { return mParentMatrix; }

	void SetParent(Matrix* value) { mParentMatrix = value; }

	Vector3 Forward();
	Vector3 Up();
	Vector3 Right();

	Vector3 GetGlobalPosition() { return mGlobalPosition; }
	Vector3 GetGlobalRotation() { return mGlobalRotation; }
	Vector3 GetGlobalScale() { return mGlobalScale; }
	Transform* GetTransform() { return this; }

	void RotateToDestinationForModel(Transform* transform, Vector3 dest); // ȸ����Ű���� �ϴ� ���� transform�� ��ǥ������ǥ.
	void RotateToDestinationForNotModel(Transform* transform, Vector3 dest); // ȸ����Ű���� �ϴ� ���� transform�� ��ǥ������ǥ.
	void MoveToDestination(Transform* transform, Vector3 dest, float moveSpeed); // �̵���Ű���� �ϴ� transform�� ��ǥ����, �̵��ӵ�.

	void ExecuteRotationPeriodFunction(function<void(Transform*, Vector3)> funcPointer, Transform* param1, Vector3 param2, float periodTime);
	void ExecuteAStarUpdateFunction(function<void(Vector3)> funcPointer, Vector3 param1, float periodTime); // ��μ��� periodTime���� �ѹ���.

	bool CheckTime(float periodTime); // periodTime �������� üũ.
	void SetHashColorBuffer();
	Float4 GetHashColor() { return mHashColor; }

private:
	void createHashColor();

public:
	bool mbIsActive;

	string mTag;

	Vector3 mPosition;
	Vector3 mRotation;
	Vector3 mScale;

	Vector3 mGlobalPosition;
	Vector3 mGlobalRotation;
	Vector3 mGlobalScale;

protected:

	Vector3 mPivot;
	Matrix mWorldMatrix;
	Matrix mLocalWorldMatrix;
	Matrix* mParentMatrix;

	MatrixBuffer* mWorldBuffer;

	float mMoveSpeed;
	float mRotationSpeed;

	vector<bool> mIsUpdateStandTimes;
	vector<float> mNextExecuteTimes;
	bool mIsAStarPathUpdate;


private:

	// Gizmos
	Material* mMaterial;
	RasterizerState* mRSState;
	ColorBuffer* mHashColorBuffer;
	Float4 mHashColor;
	
};