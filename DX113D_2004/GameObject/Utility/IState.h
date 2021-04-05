#pragma once
#define interface class

interface IState
{
public:
	IState() {};
	virtual ~IState() {};

	virtual void Enter(Mutant* mutant) = 0;
	virtual void Execute(Mutant* mutant) = 0;
	virtual void Exit(Mutant* mutant) = 0;
};