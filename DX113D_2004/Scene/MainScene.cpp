#include "Framework.h"
#include "MainScene.h"

MainScene::MainScene()
{
	terrain = new Terrain();
	player = GM->GetPlayer();
	player->SetTerrain(terrain);
	Environment::Get()->SetTargetToCamera(player);
	monsters = GM->GetMonsters();

	obstacle1 = new ModelObject("StanfordBunny/StanfordBunny", Collider::BOX);
	obstacle1->scale = { 0.1f, 0.1f, 0.1f };
	obstacle1->position = { 40, 20, 40 };
	obstacle1->UpdateWorld();
	obstacle1->GetCollider()->UpdateWorld();

	obstacle2 = new ModelObject("StanfordBunny/StanfordBunny", Collider::BOX);
	obstacle2->scale = { 0.1f, 0.1f, 0.1f };
	obstacle2->position = { 90, 20, 90 };
	obstacle2->UpdateWorld();
	obstacle2->GetCollider()->UpdateWorld();
	



	monsters[0]->position = { 50.0f, 0.0f, 50.0f };
	//monsters[1]->position = { 45.0f, 0.0f, 50.0f };

	monsters[0]->SetTerrain(terrain);
	//monsters[1]->SetTerrain(terrain);

	vector<Collider*> monsters0Obstacles;
	monsters0Obstacles.push_back(obstacle1->GetCollider());
	monsters0Obstacles.push_back(obstacle2->GetCollider());
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

	obstacle1->Render();
	obstacle2->Render();

	//monsters[0]->GetAStar()->Render();
	
	
}

void MainScene::PostRender()
{
	player->PostRender();
	ImGui::Text("mPathSize : %d", monsters[0]->GetPath().size());

}
