#pragma once




class MutantDieState : public MonsterState
{

public:
	MutantDieState();
	~MutantDieState();

	virtual void Enter(Monster* monster) override;
	virtual void Execute(Monster* monster) override;
	virtual void Exit(Monster* monster) override;

private:

};


