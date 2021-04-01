#pragma once

class Ken : public ModelAnimator, public Transform
{
private:
	const float MAX_SPEED = 10.0f;
	const float JUMP_POWER = 50.0f;

	enum State
	{
		IDLE,
		RUN,
		JUMP,
		SHOOT
	}state;

	float moveSpeed;
	float rotSpeed;

	Vector3 velocity;
	float accelation;
	float deceleration;

	float jumpPower;
	float gravity;
	bool isJump;

	bool isShoot;

	Vector3 destPos;
	vector<Vector3> path;

	Terrain* terrain;
	ModelObject* weapon;
	Matrix rightHand;
	Collider* collider;

	AStar* aStar;	
public:
	Ken();
	~Ken();

	void Update();
	void Render();
	void PostRender();

	void SetTerrain(Terrain* value) { terrain = value; }
	void SetAStar(AStar* value) { aStar = value; }
private:
	void Control();
	void Move();
	void MovePath();
	void Rotate();
	void Jump();
	void Shoot();

	void SetIdle();

	void SetAnimation(State value);

	void SetWeapon();
};