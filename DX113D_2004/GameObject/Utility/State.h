#pragma once


class MonsterState
{
public:
	MonsterState();
	virtual ~MonsterState();

	virtual void Enter(Monster* monster) = 0;
	virtual void Execute(Monster* monster) = 0;
	virtual void Exit(Monster* monster) = 0;

	void SetAttackInformation(AttackInformation attackInformation) { mAttackInformation = attackInformation; }
	void SetIsDie(bool isDie) { mbIsDie = isDie; }

protected:
	Player* mPlayer;
	function<void(Transform*, Vector3)> mPeriodFuncPointer;
	bool mbIsAttacked;
	AttackInformation mAttackInformation;
	bool mbIsDie;

};