#pragma once


class MutantAttackState : public MonsterState
{
public:
	MutantAttackState();
	~MutantAttackState();

	// State��(��) ���� ��ӵ�
	virtual void Enter(Monster* monster) override;
	virtual void Execute(Monster* monster) override;
	virtual void Exit(Monster* monster) override;

private:

};