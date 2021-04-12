#include "Framework.h"

Monster::Monster() :
	mMoveSpeed(10.0f),
	mDamage(100.0f),
	mMaxHP(1000.0f),
	mCurrentHP(0.0f),
	mAStar(nullptr),
	mTerrain(nullptr),
	mbIsStalk(false),
	mDistanceToPlayer(0.0f),
	mPlayerDetectRange(30.0f),
	mDistanceToPlayerForAttack(20.0f),
	mPlayer(nullptr)
{
	mCurrentHP = mMaxHP;
	mPatrolState = new PatrolState();
	mStalkingState = new StalkingState();
	mAttackState = new AttackState();
}

Monster::~Monster()
{
	delete mPatrolState;
	delete mStalkingState;
	delete mAttackState;
}

float Monster::GetDistanceToPlayer()
{
	mPlayer = GM->Get()->GetPlayer();
	mDistanceVector3ToPlayer = mPlayer->position - position;
	mDistanceToPlayer = sqrt(pow(mDistanceVector3ToPlayer.x, 2) + pow(mDistanceVector3ToPlayer.z, 2));

	return mDistanceToPlayer;
}






