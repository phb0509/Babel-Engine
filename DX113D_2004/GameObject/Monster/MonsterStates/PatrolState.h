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

	virtual void Enter(Mutant* mutant) override;
	virtual void Execute(Mutant* mutant) override;
	virtual void Exit(Mutant* mutant) override;

private:

	void Initialize();


private:

	// Patrol �Լ��� ���̴� ������.
	float mPatrolRandomNum; // Patrol�� ������.
	bool mbPatrolMove; // �ٽ� �����ð��� ���ؾ��ϴ°�?
	bool mbSetPatrolDest; // �����ð��� �ƴ°�?
	float mCurrentTime;
	float mStandTime;

	Vector3 mPatrolDestPos;
	Vector3 mDest;




};


