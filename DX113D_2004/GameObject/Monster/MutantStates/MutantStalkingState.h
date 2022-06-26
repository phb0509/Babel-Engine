#pragma once



class MutantStalkingState : public MonsterState
{

public:
	MutantStalkingState();
	~MutantStalkingState();

	virtual void Enter(Monster* monster) override;
	virtual void Execute(Monster* monster) override;
	virtual void Exit(Monster* monster) override;

private:
	//bool mbIsAttack;

};



