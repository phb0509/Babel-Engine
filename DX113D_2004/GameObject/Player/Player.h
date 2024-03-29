#pragma once

class Player : public PlayerModelAnimator, public Transform
{
private:

	enum eAnimationStates
	{
		Tpose,
		Idle,
		Run,
		LeftWalk,
		RightWalk,
		BackWalk,
		NormalAttack,
		KickAttack,
		Die
	}mAnimationStates;

	struct MonsterForAttackCheck
	{
		Monster* monster;
		map<string, bool> isCheckAttackMap; // 플레이어 공격이름(ex.NormalAttack) , 피격여부.
	};

public:
	Player();
	~Player();

	void Update();
	void Render();
	void DeferredRender();
	void PostRender();
	void UIRender();

	void SetTerrain(Terrain* value) { mTerrain = value; mbIsLODTerrain = false; }
	void SetLODTerrain(TerrainLOD* value) { mLODTerrain = value; mbIsLODTerrain = true; }
	void SetAStar(AStar* value) { aStar = value; }
	void SetTargetCamera(Camera* camera) { mTargetCamera = camera; }
	void SetTargetCameraInWorld(Camera* camera) { mTargetCameraInWorld = camera; }
	void SetIsTargetMode(bool value) { mbIsTargetMode = value; }
	void SetMonsters(string name, vector<Monster*> monsters);

	void OnDamage(AttackInformation attackInformation);
	bool CheckIsCollision(Collider* collider);	
	
	void renderColliders();
	void renderDeferredColliders();

private:
	void setIdle();
	void setAnimation(eAnimationStates value, float speed = 1.0f, float takeTime = 0.2f, bool isForcingPlay = false);
	void updateCamera();

	// Move
	void move();
	void moveInTargetMode(); // Move player
	void moveInWorldMode();

	void moveTargetCamera(); // Move Camera
	void moveTargetCameraInWorld(); // for Show FrustumCulling
	void rotateTargetCamera();
	void rotateInTargetMode();

	// Collider
	void loadCollidersBinaryFile(wstring fileName);
	void setColliders();

	// Attack
	void attack();
	void setAttackInformations();
	void setNormalAttackEnd();
	void setKickAttackEnd();

	void normalAttack(); // 기본공격 
	void kickAttack(); // 넉백

	void checkNormalAttackCollision();
	void checkKickAttackCollision();

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

	//map<string, vector<Monster*>> mMonsters;
	map<string, vector<MonsterForAttackCheck>> mMonsters;

	bool mbIsTargetMode;

	Vector3 mPreFrameMousePosition;
	Vector3 mTargetCameraForward;
	float mTargetCameraRotationY;
	float mTargetCameraRotationX;
	bool mbIsLODTerrain;

	float mMaxHP;
	float mCurHP;
	float mHPRate;

	float mMaxMP;
	float mCurMP;
	float mMPRate;

	bool mbIsDie;

	// Attack
	map<string, AttackInformation> mAttackInformations;
	map<string, bool> mbIsCheckAttack;

	// NormalAttack
	bool mbIsNormalAttack;
	float mNormalAttackDamage;
	bool mbIsCheckNormalAttackCollision;

	// KickAttack
	bool mbIsKickAttack;
	float mKickAttackDamage;
	bool mbIsCheckKickAttackCollision;

	// UI
	PlayerStatusBar* mStatusBar;
};