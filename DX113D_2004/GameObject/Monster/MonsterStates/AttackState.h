#pragma once

class AttackState : public State
{
public:
	AttackState();
	~AttackState();

	// State을(를) 통해 상속됨
	virtual void Enter(Monster* monster) override;
	virtual void Execute(Monster* monster) override;
	virtual void Exit(Monster* monster) override;

private:
	bool mbIsAttacking;
};