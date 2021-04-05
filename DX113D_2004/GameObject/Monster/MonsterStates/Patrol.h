#pragma once



//Patrol,
//Stalk,
//Attack, // 스매쉬어택이든 뭐든, 공격상태면 일단?
//OnDamage,
//Die,

class Patrol : public IState
{

public:
	Patrol();
	~Patrol();

	virtual void Enter(Mutant* mutant) override;
	virtual void Execute(Mutant* mutant) override;
	virtual void Exit(Mutant* mutant) override;

private:

	void Initialize();


private:

	// Patrol 함수에 쓰이는 변수들.
	float mPatrolRandomNum; // Patrol용 랜덤값.
	bool mbPatrolMove; // 다시 정찰시간을 정해야하는가?
	bool mbSetPatrolDest; // 정찰시간이 됐는가?
	float mCurrentTime;
	float mStandTime;

	Vector3 mPatrolDestPos;
	Vector3 mDest;

	float mStalkingRangeRadius; // 플레이어 인지범위. 
	Vector3 mDistanceVector3ToPlayer; // 플레이어까지 거리 x,z만 반영.
	float mDistanceToPlayer;

};


