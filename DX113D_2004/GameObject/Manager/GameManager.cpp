#include "Framework.h"

GameManager::GameManager()
{
	mPlayer = new Player();

	for (int i = 0; i < 1; i++)
	{
		mMonster = new Mutant;
		mMonsters.push_back(mMonster);
	}

	for (int i = 0; i < mMonsters.size(); i++)
	{
		mMonsters[i]->mTag = to_string(i) + "번 몬스터";
		mMonstersHitCheck[mMonsters[i]] = false;
	}
}

GameManager::~GameManager()
{
	delete mPlayer;
	
	for (int i = 0; i < mMonsters.size(); i++)
	{
		delete mMonsters[i];
		mMonsters[i] = nullptr;
	}

}

void GameManager::PlayDropEvents()
{
	for (int i = 0; i < mWindowDropEvents.size(); i++)
	{
		mWindowDropEvents[i]();
	}
}


