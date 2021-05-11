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
	void initialize();
	void setIdle();
	void setAnimation(State value);
	void input();
	void move();
	void worldCameraMove();
	void setColliders();
	void loadCollider();
	void findCollider(string name, Collider* collider);
	void rotate();
	void checkNormalAttackCollision();



	void setAttackEnd();
	void normalAttack();

private:
	bool isInitialize;
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
	

	bool mIsNormalAttack;
	bool mIsNormalAttackCollide;
	float normalAttackDamage;

	bool check1;

};