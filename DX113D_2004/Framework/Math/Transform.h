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

	Matrix* GetWorld() { return &world; }
	void SetParent(Matrix* value) { parent = value; }

	Vector3 Forward();
	Vector3 Up();
	Vector3 Right();

	Vector3 GlobalPos() { return globalPosition; }
	Vector3 GlobalRot() { return globalRotation; }
	Vector3 GlobalScale() { return globalScale; }

	void RotateToDestination(Transform* transform, Vector3 dest); // 회전시키고자 하는 transform과 목표지점좌표.
	void MoveToDestination(Transform* transform, Vector3 dest, float moveSpeed); // 이동시키고자 하는 transform과 목표지점, 이동속도.

	void ExecuteRotationPeriodFunction(function<void(Transform*, Vector3)> funcPointer, Transform* param1, Vector3 param2, float periodTime);
	void ExecuteAStarUpdateFunction(function<void(Vector3)> funcPointer, Vector3 param1, float periodTime); // 경로설정 periodTime마다 한번씩.

	bool CheckTime(float periodTime); // periodTime 지났는지 체크.

	Transform* GetTransform() { return this; }

private:
	void CreateAxis();



public:
	static bool isAxisDraw;

	bool isActive;

	string tag;

	Vector3 position;
	Vector3 rotation;
	Vector3 scale;

protected:
	Vector3 globalPosition;
	Vector3 globalRotation;
	Vector3 globalScale;

	Vector3 pivot;

	Matrix world;
	Matrix* parent;

	MatrixBuffer* worldBuffer;

	float mMoveSpeed;
	float mRotationSpeed;

	vector<bool> mIsUpdateStandTimes;
	vector<float> mNextExecuteTimes;
	bool mIsAStarPathUpdate;

private:
	Material* material;
	Mesh* mesh;

	MatrixBuffer* transformBuffer;

	vector<VertexColor> vertices;
	vector<UINT> indices;


	
};