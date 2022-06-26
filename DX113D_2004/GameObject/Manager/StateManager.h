#pragma once

class StateManager : public Singleton<StateManager>
{
private:
	friend class Singleton;

	StateManager();
	~StateManager();

public:


private:
	
	MutantPatrolState* mMutantPatrolState;
	MutantStalkingState* mMutantStalkingState;
	MutantAttackState* mMutantAttackState;
	MutantOnDamageState* mMutantOnDamageState;
	MutantDieState* mMutantDieState;
};