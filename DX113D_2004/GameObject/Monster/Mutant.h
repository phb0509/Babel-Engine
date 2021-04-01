#pragma once

class Mutant : public Monster, public ModelAnimator
{
public:
	friend class Player;

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

	enum State
	{
		IDLE,
		RUN,
		SMASHATTACK,
		ONDAMAGE,
		DIE
	}state;

	Mutant();
	~Mutant();


	virtual void Update() override;
	virtual void Render() override;
	void PostRender();

	virtual Collider* GetHitCollider() override; 
	virtual void OnDamage(float damage) override;
	virtual void CheckOnHit() override;

private:
	void Move();

	void SetIdle();
	void SetAnimation(State value);
	void SetOnDamageEnd();
	void SetColliders();
	void LoadCollider();
	void FindCollider(string name, Collider* collider);


private:

	Player* player;
	Collider* bodyCollider;
	Matrix bodyMatrix;

	Collider* smashAttackCollider;
	Matrix smashAttackMatrix;

	vector<colliderData> colliderDatas;
	vector<temp_colliderData> temp_colliderDatas;

	bool isOnHit;
};