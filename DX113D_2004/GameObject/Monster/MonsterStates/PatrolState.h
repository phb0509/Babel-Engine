#pragma once



//Patrol,
//Stalk,
//Attack, // 스매쉬어택이든 뭐든, 공격상태면 일단?
//OnDamage,
//Die,

class PatrolState : public State
{

public:
	PatrolState();
	~PatrolState();

	virtual void Enter(Monster* mMonster) override;
	virtual void Execute(Monster* mMonster) override;
	virtual void Exit(Monster* mMonster) override;

private:

	void Initialize();
	void SetPatrolTargetPoint(Vector3& patrolTargetPoint);

private:

	// Patrol 함수에 쓰이는 변수들.
	float mPatrolRandomNum; // Patrol용 랜덤값.
	bool mbIsPatrolMove; // 다시 정찰시간을 정해야하는가?
	bool mbIsSetPatrolDest; // 정찰시간이 됐는가?
	float mCurrentTime;
	float mStandTime;

	Vector3 mPatrolDestPos;
	float mMaxPatrolWidth;
	float mMaxPatrolHeight;
};


