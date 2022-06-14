#pragma once

class Player : public ModelAnimator, public Transform
{
private:

	enum State
	{
		TPOSE,
		IDLE,
		RUN,
		ATTACK,
		DIE
	}state;

	struct ColliderData
	{
		string colliderName;
		string nodeName;
		UINT colliderType;

		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
	};

	struct TempCollider
	{
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
	};

	struct SettedCollider
	{
		string colliderName;
		string nodeName;
		Matrix matrix;
		Collider* collider;
	};

public:
	Player();
	~Player();

	void Update();
	void Render();
	void DeferredRender();
	void PostRender();
	void RenderColliders();

	void SetTerrain(Terrain* value) { mTerrain = value; mbIsLODTerrain = false; }
	void SetLODTerrain(TerrainLOD* value) { mLODTerrain = value; mbIsLODTerrain = true; }
	void SetAStar(AStar* value) { aStar = value; }
	void SetTargetCamera(Camera* camera) { mTargetCamera = camera; }
	void SetTargetCameraInWorld(Camera* camera) { mTargetCameraInWorld = camera; }
	void SetIsTargetMode(bool value) { mbIsTargetMode = value; }
	void SetMonsters(vector<Monster*> monsters) { mMonsters = monsters; }

private:
	void initialize();
	void setIdle();
	void setAnimation(State value);
	void move();
	void attack();
	void updateCamera();

	void moveInTargetMode(); // Move player
	void moveInWorldMode();

	void moveTargetCamera(); // Move Camera
	void moveTargetCameraInWorld(); // for Show FrustumCulling
	void rotateTargetCamera();

	void setColliders();
	void loadBinaryCollidersFile(wstring fileName);
	void rotateInTargetMode();
	void checkNormalAttackCollision();
	void setAttackEnd();
	void normalAttack();

private:
	Terrain* mTerrain;
	TerrainLOD* mLODTerrain;
	AStar* aStar;
	Camera* mTargetCamera;
	Camera* mTargetCameraInWorld; // for Show

	vector<TempCollider> mTempColliderSRTdatas;
	vector<ColliderData> mTempColliderDatas;
	vector<SettedCollider> mColliders;
	map<string, Collider*> mCollidersMap;

	vector<Monster*> mMonsters;
	
	bool mbIsInitialize;
	bool mbIsNormalAttack;
	bool mbIsNormalAttackCollide;
	float mNormalAttackDamage;
	bool mbIsTargetMode;

	Vector3 mPreFrameMousePosition;
	Vector3 mTargetCameraForward;
	float mTargetCameraRotationY;
	float mTargetCameraRotationX;
	bool mbIsLODTerrain;
};