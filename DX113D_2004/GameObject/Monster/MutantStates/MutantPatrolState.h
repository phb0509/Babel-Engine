#pragma once


class MutantPatrolState : public MonsterState
{

public:
	MutantPatrolState();
	~MutantPatrolState();

	virtual void Enter(Monster* monster) override;
	virtual void Execute(Monster* monster) override;
	virtual void Exit(Monster* monster) override;

private:
	void Initialize();
	void SetPatrolTargetPoint(Vector3& patrolTargetPoint);

private:
	float mPatrolRandomNum; // Patrol용 랜덤값.
	bool mbIsPatrolMove; // 다시 정찰시간을 정해야하는가?
	bool mbIsSetPatrolDest; // 정찰시간이 됐는가?
	float mCurrentTime;
	float mStandTime;

	Vector3 mPatrolDestPos;
	float mMaxPatrolWidth;
	float mMaxPatrolHeight;
};


