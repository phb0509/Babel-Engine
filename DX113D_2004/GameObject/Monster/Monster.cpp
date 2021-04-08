#include "Framework.h"

Monster::Monster() :
	mMoveSpeed(10.0f),
	mDamage(100.0f),
	mMaxHP(1000.0f),
	mCurrentHP(0.0f),
	mAStar(nullptr),
	mTerrain(nullptr),
	isStalk(false),
	mPlayerDetectRange(30.0f),
	mDistanceBetweenPlayerAndMonsterForAttack(10.0f)
{
	
	mCurrentHP = mMaxHP;
}

Monster::~Monster()
{
}






