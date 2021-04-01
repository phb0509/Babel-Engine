#include "Framework.h"

Monster::Monster() : moveSpeed(100.0f), damage(100.0f), maxHP(1000.0f), currentHP(0.0f)
{
	currentHP = maxHP;
}

Monster::~Monster()
{
}


