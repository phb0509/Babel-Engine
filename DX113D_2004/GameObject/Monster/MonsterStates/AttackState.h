#pragma once

class AttackState : public State
{
public:

	AttackState();
	~AttackState();

	// State을(를) 통해 상속됨
	virtual void Enter(Mutant* mutant) override;
	virtual void Execute(Mutant* mutant) override;
	virtual void Exit(Mutant* mutant) override;

private:

};