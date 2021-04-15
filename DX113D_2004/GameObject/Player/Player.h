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

	struct colliderData
	{
		string name;
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
	};

	struct temp_colliderData
	{
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
	};

public:
	Player();
	~Player();

	void Update();
	void Render();
	void PostRender();

	void SetTerrain(Terrain* value) { terrain = value; }
	void SetAStar(AStar* value) { aStar = value; }


private:
	void Initialize();
	void SetIdle();
	void SetAnimation(State value);
	void Input();
	void Move();
	void SetColliders();
	void LoadCollider();
	void FindCollider(string name, Collider* collider);
	void Rotate();
	void CheckNormalAttackCollision();


	void SetAttackEnd();
	void NormalAttack();

private:
	bool isInitialize;
	float mMoveSpeed;
	Terrain* terrain;
	AStar* aStar;

	vector<temp_colliderData> temp_colliderDatas;
	vector<colliderData> colliderDatas;

	Matrix swordMatrix;
	Collider* swordCollider;

	Matrix bodyMatrix;
	Collider* bodyCollider;
	Vector3 oldPos;
	vector<Monster*> monsters;
	

	bool isNormalAttack;
	bool isNormalAttackCollide;
	float normalAttackDamage;

	bool check1;

};