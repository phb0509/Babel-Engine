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


	Terrain* GetTerrain()const { return mTerrain; }
	vector<Vector3>& GetPath() { return mPath; }
	float GetMoveSpeed() const { return mMoveSpeed; }
	bool GetIsStalk()const { return isStalk; }
	float GetDistanceBetweenPlayerAndMonsterForAttack() const { return mDistanceBetweenPlayerAndMonsterForAttack; }


	void SetTerrain(Terrain* value) { mTerrain = value; }
	void SetAStar(AStar* value) { mAStar = value; }
	void SetIsStalk(bool value) { isStalk = value; }
	void SetDistanceBetweenPlayerAndMonsterForAttack(float value) { mDistanceBetweenPlayerAndMonsterForAttack = value; }



protected:
	float mMoveSpeed;
	float mDamage;
	float mMaxHP;
	float mCurrentHP;

	Terrain* mTerrain;
	AStar* mAStar;
	Vector3 mDestPos;
	vector<Vector3> mPath;

	bool isStalk;

	float mPlayerDetectRange;
	float mDistanceBetweenPlayerAndMonsterForAttack;
};