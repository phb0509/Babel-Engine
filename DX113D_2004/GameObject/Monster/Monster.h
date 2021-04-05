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


	Terrain* GetTerrain() { return mTerrain; }
	vector<Vector3>& GetPath() { return mPath; }
	float GetMoveSpeed() { return mMoveSpeed; }
	bool GetIsStalk() { return isStalk; }

	void SetTerrain(Terrain* value) { mTerrain = value; }
	void SetAStar(AStar* value) { mAStar = value; }
	void SetIsStalk(bool value) { isStalk = value; }


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

public:


};