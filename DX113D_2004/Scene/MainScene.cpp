#include "Framework.h"
#include "MainScene.h"

MainScene::MainScene()
{
	terrain = new Terrain();
	player = GM->GetPlayer();
	player->SetTerrain(terrain);
	CAMERA->SetPlayer(player);
	monsters = GM->GetMonsters();
	
	monsters[0]->position = { 50.0f, 0.0f, 50.0f };
	monsters[1]->position = { 45.0f, 0.0f, 50.0f };

	monsters[0]->SetTerrain(terrain);
	monsters[1]->SetTerrain(terrain);
	
}

MainScene::~MainScene()
{
	delete terrain;
}

void MainScene::Update()
{
	terrain->Update();
	player->Update();
	
	for (int i = 0; i < monsters.size(); i++)
	{
		monsters[i]->Update();
	}
}

void MainScene::PreRender()
{
}

void MainScene::Render()
{
	terrain->Render();
	player->Render();

	for (int i = 0; i < monsters.size(); i++)
	{
		monsters[i]->Render();
	}
}

void MainScene::PostRender()
{
	player->PostRender();
}
