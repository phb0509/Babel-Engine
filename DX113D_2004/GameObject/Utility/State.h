#pragma once


class State
{
public:
	State();
	virtual ~State();

	virtual void Enter(Monster* monster) = 0;
	virtual void Execute(Monster* monster) = 0;
	virtual void Exit(Monster* monster) = 0;

protected:
	Player* mPlayer;
	function<void(Transform*, Vector3)> mPeriodFuncPointer;
};