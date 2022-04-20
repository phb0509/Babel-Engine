#include "Framework.h"

GameManager::GameManager()
{
	mPlayer = new Player();
}

GameManager::~GameManager()
{
	delete mPlayer;
}

void GameManager::PlayDropEvents()
{
	for (int i = 0; i < mWindowDropEvents.size(); i++)
	{
		mWindowDropEvents[i]();
	}
}


