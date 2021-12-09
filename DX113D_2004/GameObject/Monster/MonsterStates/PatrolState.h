#pragma once



//Patrol,
//Stalk,
//Attack, // ���Ž������̵� ����, ���ݻ��¸� �ϴ�?
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

	// Patrol �Լ��� ���̴� ������.
	float mPatrolRandomNum; // Patrol�� ������.
	bool mbPatrolMove; // �ٽ� �����ð��� ���ؾ��ϴ°�?
	bool mbSetPatrolDest; // �����ð��� �ƴ°�?
	float mCurrentTime;
	float mStandTime;

	Vector3 mPatrolDestPos;
};


