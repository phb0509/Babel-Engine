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

	virtual void Enter(Monster* mMonster) override;
	virtual void Execute(Monster* mMonster) override;
	virtual void Exit(Monster* mMonster) override;

private:
	bool mbIsAttack;


};


