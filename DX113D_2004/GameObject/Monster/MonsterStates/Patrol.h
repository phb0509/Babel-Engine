#pragma once



//Patrol,
//Stalk,
//Attack, // ���Ž������̵� ����, ���ݻ��¸� �ϴ�?
//OnDamage,
//Die,

class Patrol : public IState
{

public:
	Patrol();
	~Patrol();

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

	float mStalkingRangeRadius; // �÷��̾� ��������. 
	Vector3 mDistanceVector3ToPlayer; // �÷��̾���� �Ÿ� x,z�� �ݿ�.
	float mDistanceToPlayer;

};


