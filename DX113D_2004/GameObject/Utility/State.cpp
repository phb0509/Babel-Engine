#include "Framework.h"

MonsterState::MonsterState():
	mPeriodFuncPointer(nullptr),
	mPlayer(nullptr),
	mbIsAttacked(false)
{
}

MonsterState::~MonsterState()
{

}
