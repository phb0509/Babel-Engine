#pragma once


class State
{
public:
	State();
	virtual ~State();

	virtual void Enter(Monster* mMonster) = 0;
	virtual void Execute(Monster* mMonster) = 0;
	virtual void Exit(Monster* mMonster) = 0;

protected:
	Player* mPlayer;
	function<void(Transform*, Vector3)> mPeriodFuncPointer;
	bool mbIsAttacked;
};