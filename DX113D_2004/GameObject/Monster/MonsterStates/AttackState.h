#pragma once

class AttackState : public State
{
public:

	AttackState();
	~AttackState();

	// State��(��) ���� ��ӵ�
	virtual void Enter(Mutant* mutant) override;
	virtual void Execute(Mutant* mutant) override;
	virtual void Exit(Mutant* mutant) override;

private:

};