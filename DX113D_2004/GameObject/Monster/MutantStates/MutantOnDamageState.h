#pragma once




class MutantOnDamageState : public MonsterState
{

public:
	MutantOnDamageState();
	~MutantOnDamageState();

	virtual void Enter(Monster* monster) override;
	virtual void Execute(Monster* monster) override;
	virtual void Exit(Monster* monster) override;

private:

};


