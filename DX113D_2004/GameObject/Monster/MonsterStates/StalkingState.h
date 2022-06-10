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

	virtual void Enter(Monster* monster) override;
	virtual void Execute(Monster* monster) override;
	virtual void Exit(Monster* monster) override;

private:
	//bool mbIsAttack;

};


