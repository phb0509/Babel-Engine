#pragma once



//Patrol,
//Stalk,
//Attack, // ���Ž������̵� ����, ���ݻ��¸� �ϴ�?
//OnDamage,
//Die,

class StalkingState : public State
{

public:
	StalkingState();
	~StalkingState();

	virtual void Enter(Mutant* mutant) override;
	virtual void Execute(Mutant* mutant) override;
	virtual void Exit(Mutant* mutant) override;

private:
	bool mbIsAttack;

};


