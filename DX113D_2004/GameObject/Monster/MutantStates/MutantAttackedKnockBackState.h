#pragma once




class MutantAttackedKnockBackState : public MonsterState
{

public:
	MutantAttackedKnockBackState();
	~MutantAttackedKnockBackState();

	virtual void Enter(Monster* monster) override;
	virtual void Execute(Monster* monster) override;
	virtual void Exit(Monster* monster) override;

private:
	float mKnockBackSpeed;
	float mReductionSpeed;
};


