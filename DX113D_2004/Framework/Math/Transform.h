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
	void RenderGizmos();
	void PreRenderGizmosForColorPicking();
	void SetWorldBuffer(UINT slot = 0);

	Matrix* GetWorldMatrix() { return &mWorldMatrix; }
	Matrix GetWorldMatrixTemp() { return mWorldMatrix; }
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

	void SetColorBuffer();

	Float4 GetHashColor() { return mHashColorForBuffer; }

	Float4 GetGizmosHashColorX() { return mGizmosHashColor.x; }
	Float4 GetGizmosHashColorY() { return mGizmosHashColor.y; }
	Float4 GetGizmosHashColorZ() { return mGizmosHashColor.z; }
	
	
private:
	void createHashColor();
	void createGizmos();
	void createGizmoseHashColor();

public:
	static bool mbIsRenderGizmos;

	bool mbIsActive;

	string mTag;

	Vector3 mPosition;
	Vector3 mRotation;
	Vector3 mScale;

protected:
	Vector3 mGlobalPosition;
	Vector3 mGlobalRotation;
	Vector3 mGlobalScale;

	Vector3 mPivot;

	Matrix mWorldMatrix;
	Matrix* mParentMatrix;

	MatrixBuffer* mWorldBuffer;

	float mMoveSpeed;
	float mRotationSpeed;

	vector<bool> mIsUpdateStandTimes;
	vector<float> mNextExecuteTimes;
	bool mIsAStarPathUpdate;


private:

	// Gizmos
	Material* mGizmosMaterial;
	Mesh* mGizmosMesh;
	MatrixBuffer* mGizmosWorldBuffer;
	vector<VertexColor> mGizmosVertices;
	vector<UINT> mGizmosIndices;
	RasterizerState* mRSState;
	ColorBuffer* mColorBuffer;
	Float4 mHashColorForBuffer;
	GizmosHashColor mGizmosHashColor;
	
};