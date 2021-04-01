#pragma once

class Monster : public Transform
{
public:
	Monster();
	~Monster();

	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual Collider* GetHitCollider() = 0;
	virtual void OnDamage(float damage) = 0;
	virtual void CheckOnHit() = 0;
	void SetTerrain(Terrain* value) { terrain = value; }
	void SetAStar(AStar* value) { aStar = value; }

protected:
	float moveSpeed;
	float damage;
	float maxHP;
	float currentHP;

	Terrain* terrain;
	AStar* aStar;
	
};