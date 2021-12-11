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
	void PostRender();

	void SetTerrain(Terrain* value) { mTerrain = value; }
	void SetAStar(AStar* value) { aStar = value; }


private:
	void initialize();
	void setIdle();
	void setAnimation(State value);
	void input();
	void moveInTargetCamera();
	void moveInWorldCamera();
	void setColliders();
	void loadBinaryFile();
	void rotate();
	void checkNormalAttackCollision();

	void setAttackEnd();
	void normalAttack();

private:
	bool mbIsInitialize;
	Terrain* mTerrain;
	AStar* aStar;

	vector<TempCollider> mColliderSRTdatas;
	vector<ColliderData> mColliderDatas;
	vector<SettedCollider> mColliders;
	map<string, Collider*> mCollidersMap;

	vector<Monster*> mMonsters;
	
	bool mbIsNormalAttack;
	bool mbIsNormalAttackCollide;
	float mNormalAttackDamage;

};