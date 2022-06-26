#pragma once




class MutantOnDamageState : public MonsterState
{

public:
	MutantOnDamageState();
	~MutantOnDamageState();

	virtual void Enter(Monster* mMonster) override;
	virtual void Execute(Monster* mMonster) override;
	virtual void Exit(Monster* mMonster) override;

private:

};


