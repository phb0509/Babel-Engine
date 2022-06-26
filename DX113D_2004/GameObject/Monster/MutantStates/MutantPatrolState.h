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
	float mPatrolRandomNum; // Patrol�� ������.
	bool mbIsPatrolMove; // �ٽ� �����ð��� ���ؾ��ϴ°�?
	bool mbIsSetPatrolDest; // �����ð��� �ƴ°�?
	float mCurrentTime;
	float mStandTime;

	Vector3 mPatrolDestPos;
	float mMaxPatrolWidth;
	float mMaxPatrolHeight;
};


