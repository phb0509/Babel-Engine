#pragma once

class Player;
class State;
class PatrolState;
class StalkingState;
class AttackState;

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
	virtual Collider* GetColliderForAStar() = 0;
	virtual void SetAnimation(eAnimation value) = 0;

	void SetAStarPath(Vector3 destPos);
	void MoveToDestUsingAStar(Vector3 dest);
	void ChangeState(State* nextState);

	Terrain* GetTerrain()const { return mTerrain; }
	vector<Vector3>& GetPath() { return mPath; }
	float GetMoveSpeed() const { return mMoveSpeed; }
	bool GetIsStalk()const { return mbIsStalk; }
	float GetDistanceToPlayer();
	float GetDistanceToPlayerForAttack() const { return mDistanceToPlayerForAttack; }
	float GetPlayerDetectRange() const { return mPlayerDetectRange; }
	PatrolState* GetPatrolState() { return mPatrolState; }
	StalkingState* GetStalkingState() { return mStalkingState; }
	AttackState* GetAttackState() { return mAttackState; }
	AStar* GetAStar() { return mAStar; }



	void SetTerrain(Terrain* value);
	void SetAStar(AStar* value) { mAStar = value; }
	void SetIsStalk(bool value) { mbIsStalk = value; }
	void SetDistanceToPlayerForAttack(float value) { mDistanceToPlayerForAttack = value; }



protected:
	float mMoveSpeed;
	float mDamage;
	float mMaxHP;
	float mCurrentHP;

	Terrain* mTerrain;
	Player* mPlayer;
	AStar* mAStar;
	Vector3 mDestPos;
	vector<Vector3> mPath;

	bool mbIsStalk;

	Vector3 mDistanceVector3ToPlayer; // 플레이어까지 거리 x,z만 반영.
	float mDistanceToPlayer;
	float mPlayerDetectRange;
	float mDistanceToPlayerForAttack;


	State* mCurrentState;
	PatrolState* mPatrolState;
	StalkingState* mStalkingState;
	AttackState* mAttackState;

	function<void(Vector3)> mPeriodFuncPointer;
	ModelAnimator* mModelAnimator;

};