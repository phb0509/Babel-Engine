#pragma once




class OnDamageState : public State
{

public:
	OnDamageState();
	~OnDamageState();

	virtual void Enter(Monster* mMonster) override;
	virtual void Execute(Monster* mMonster) override;
	virtual void Exit(Monster* mMonster) override;

private:


private:


};


