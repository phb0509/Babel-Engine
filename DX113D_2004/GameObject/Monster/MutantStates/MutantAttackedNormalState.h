#pragma once




class MutantAttackedNormalState : public MonsterState
{

public:
	MutantAttackedNormalState();
	~MutantAttackedNormalState();

	virtual void Enter(Monster* monster) override;
	virtual void Execute(Monster* monster) override;
	virtual void Exit(Monster* monster) override;

private:

};


