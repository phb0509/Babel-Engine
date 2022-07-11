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
	Matrix& GetWorldMatrixValue() { return mWorldMatrix; }
	Matrix GetInverseWorldMatrix() { return XMMatrixInverse(nullptr, mWorldMatrix); }
	Matrix GetLocalWorldMatrix() { return mLocalWorldMatrix; }
	Matrix* GetParentMatrix() { return mParentMatrix; }
	Vector3 GetGlobalPosition() { return mGlobalPosition; }
	Vector3 GetGlobalRotation() { return mGlobalRotation; }
	Vector3 GetGlobalScale() { return mGlobalScale; }
	Transform* GetTransform() { return this; }
	Vector3 GetForwardVector();
	Vector3 GetUpVector();
	Vector3 GetRightVector();
	string GetTag() { return mTag; }
	Float4 GetHashColor() { return mHashColor; }
	bool GetIsInFrustum() { return mbIsInFrustum; }
	bool GetIsActive() { return mbIsActive; }

	void SetParent(Matrix* value) { mParentMatrix = value; }


	void RotateToDestinationForModel(Transform* transform, Vector3 dest); // 회전시키고자 하는 모델의 transform과 목표지점좌표.
	void RotateToDestinationForNotModel(Transform* transform, Vector3 dest); // 회전시키고자 하는 모델의 transform과 목표지점좌표.
	void MoveToDestination(Transform* transform, Vector3 dest, float moveSpeed); // 이동시키고자 하는 transform과 목표지점, 이동속도.

	void ExecuteRotationPeriodFunction(function<void(Transform*, Vector3)> funcPointer, Transform* param1, Vector3 param2, float periodTime);
	void ExecuteAStarUpdateFunction(function<void(Vector3)> funcPointer, Vector3 param1, float periodTime); // 경로설정 periodTime마다 한번씩.

	bool CheckTime(float periodTime); // periodTime 지났는지 체크.

	void SetHashColorBuffer();
	void SetTag(string tag) { mTag = tag; }
	void SetIsInFrustum(bool isInFrustum) { mbIsInFrustum = isInFrustum; }
	void SetIsActive(bool isActive) { mbIsActive = isActive; }

private:
	void createHashColor();

public:
	Vector3 mPosition;
	Vector3 mRotation;
	Vector3 mScale;

	Vector3 mGlobalPosition;
	Vector3 mGlobalRotation;
	Vector3 mGlobalScale;

protected:
	string mTag;
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
	bool mbIsInFrustum;
	bool mbIsActive;
	bool mbIsRender;

private:
	// Gizmos
	Material* mMaterial;
	RasterizerState* mRSState;
	ColorBuffer* mHashColorBuffer;
	Float4 mHashColor;

};