#pragma once

class Krillin : public ModelAnimator, public Transform
{
private:
	enum State
	{
		IDLE,
		RUN,
		ATTACK
	}state;

	float moveSpeed;
	float rotSpeed;

	Vector3 direction;
	Vector3 destPos;

	bool isMove;

	Terrain* terrain;

	Transform offset;

	Collider* collider;
public:
	Krillin();
	~Krillin();

	void Update();
	void Render();

	void KeyboardMove();
	void MouseMove();
	void Attack();

	void AttackEnd();

	void SetAction(State state);
	void SetTerrain(Terrain* terrain) { this->terrain = terrain; }

	Collider* GetCollider() { return collider; }
};