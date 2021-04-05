#pragma once



//Patrol,
//Stalk,
//Attack, // 스매쉬어택이든 뭐든, 공격상태면 일단?
//OnDamage,
//Die,

class Stalking : public IState
{

public:
	Stalking();
	~Stalking();

	virtual void Enter(Mutant* mutant) override;
	virtual void Execute(Mutant* mutant) override;
	virtual void Exit(Mutant* mutant) override;

private:


private:

};


