#pragma once

class Monster;

class Transform
{
public:
	Transform(string tag = "Untagged");
	virtual ~Transform();

	void UpdateWorld();
	void RenderAxis();

	void SetWorldBuffer(UINT slot = 0);

	Matrix* GetWorld() { return &mWorldMatrix; }
	void SetParent(Matrix* value) { mParentMatrix = value; }

	Vector3 Forward();
	Vector3 Up();
	Vector3 Right();

	Vector3 GetGlobalPosition() { return mGlobalPosition; }
	Vector3 GetGlobalRotation() { return mGlobalRotation; }
	Vector3 GetGlobalScale() { return mGlobalScale; }

	void RotateToDestinationForModel(Transform* transform, Vector3 dest); // ȸ����Ű���� �ϴ� ���� transform�� ��ǥ������ǥ.
	void RotateToDestinationForNotModel(Transform* transform, Vector3 dest); // ȸ����Ű���� �ϴ� ���� transform�� ��ǥ������ǥ.
	void MoveToDestination(Transform* transform, Vector3 dest, float moveSpeed); // �̵���Ű���� �ϴ� transform�� ��ǥ����, �̵��ӵ�.

	void ExecuteRotationPeriodFunction(function<void(Transform*, Vector3)> funcPointer, Transform* param1, Vector3 param2, float periodTime);
	void ExecuteAStarUpdateFunction(function<void(Vector3)> funcPointer, Vector3 param1, float periodTime); // ��μ��� periodTime���� �ѹ���.

	bool CheckTime(float periodTime); // periodTime �������� üũ.

	Transform* GetTransform() { return this; }

private:
	void createAxis();


public:
	static bool mbIsAxisDrawing;

	bool isActive;

	string tag;

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
	Material* mMaterial;
	Mesh* mMesh;

	MatrixBuffer* mTransformBuffer;

	vector<VertexColor> mVertices;
	vector<UINT> mIndices;

};