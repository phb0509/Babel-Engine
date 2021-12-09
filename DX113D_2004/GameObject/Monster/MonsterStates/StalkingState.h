#pragma once



//Patrol,
//Stalk,
//Attack, // 스매쉬어택이든 뭐든, 공격상태면 일단?
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


