#pragma once


class State
{
public:
	State();
	virtual ~State();

	virtual void Enter(Mutant* mutant) = 0;
	virtual void Execute(Mutant* mutant) = 0;
	virtual void Exit(Mutant* mutant) = 0;

protected:
	Player* mPlayer;
	function<void(Transform*, Vector3)> mPeriodFuncPointer;
};