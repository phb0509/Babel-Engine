#include "Framework.h"

GameManager::GameManager()
{
	mPlayer = new Player();

	for (int i = 0; i < 0; i++)
	{
		mMonster = new Mutant;
		mMutants.push_back(mMonster);
	}

	for (int i = 0; i < mMutants.size(); i++)
	{
		mMutants[i]->mTag = to_string(i) + "번 몬스터";
		mMonstersHitCheck[mMutants[i]] = false;
	}
}

GameManager::~GameManager()
{
	delete mPlayer;
	
	for (int i = 0; i < mMutants.size(); i++)
	{
		delete mMutants[i];
		mMutants[i] = nullptr;
	}

}

void GameManager::PlayDropEvents()
{
	for (int i = 0; i < mWindowDropEvents.size(); i++)
	{
		mWindowDropEvents[i]();
	}
}


