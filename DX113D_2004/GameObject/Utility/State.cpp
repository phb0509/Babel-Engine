#include "Framework.h"

State::State():
	mPeriodFuncPointer(nullptr),
	mPlayer(nullptr),
	mbIsAttacked(false)
{
}

State::~State()
{

}
