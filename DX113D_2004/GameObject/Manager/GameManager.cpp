#include "Framework.h"

GameManager::GameManager() :
	mbIsHoveredAssetBrowserWindow(false)
{
	mPlayer = new Player();
}

GameManager::~GameManager()
{
	SafeDelete(mPlayer);
}

void GameManager::ExecuteFileDropEvents(int eventIndex)
{
	// 0. Program::PostRender
	// 1. ColliderSettingScene::playAssetWindowDropEvent

	mWindowDropEvents[eventIndex]();
}


