#include "Framework.h"
#include "MainScene.h"

MainScene::MainScene()
{
	terrain = new Terrain();
	player = GM->GetPlayer();
	player->SetTerrain(terrain);
	CAMERA->SetPlayer(player);
	monsters = GM->GetMonsters();

	obstacle = new ModelObject("StanfordBunny/StanfordBunny", Collider::BOX);
	obstacle->scale = { 0.4f, 0.4f, 0.4f };
	obstacle->position = { 120, 20, 120 };
	obstacle->UpdateWorld();
	obstacle->GetCollider()->UpdateWorld();
	



	monsters[0]->position = { 50.0f, 0.0f, 50.0f };
	//monsters[1]->position = { 45.0f, 0.0f, 50.0f };

	monsters[0]->SetTerrain(terrain);
	//monsters[1]->SetTerrain(terrain);

	vector<Collider*> monsters0Obstacles;
	monsters0Obstacles.push_back(obstacle->GetCollider());
	//vector<Collider*> monsters1Obstacles;

	//monsters0Obstacles.push_back(monsters[1]->GetColliderForAStar());
	//monsters1Obstacles.push_back(monsters[0]->GetColliderForAStar());

	monsters[0]->GetAStar()->SetObstacle(monsters0Obstacles);
	//monsters[1]->GetAStar()->SetObstacle(monsters1Obstacles);

	
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

	obstacle->Render();
	
	
}

void MainScene::PostRender()
{
	player->PostRender();
}
