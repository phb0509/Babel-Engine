#include "Framework.h"
#include "MainScene.h"

MainScene::MainScene():
	mTargetCameraFrustum(nullptr)
{
	mTerrain = new Terrain();
	mPlayer = GM->GetPlayer();
	mPlayer->SetTerrain(mTerrain);
	Environment::Get()->SetTargetToCamera(mPlayer);
	monsters = GM->GetMonsters();

	mObstacle1 = new ModelObject("StanfordBunny/StanfordBunny", Collider::BOX);
	mObstacle1->mScale = { 0.1f, 0.1f, 0.1f };
	mObstacle1->mPosition = { 40, 20, 40 };
	mObstacle1->UpdateWorld();
	mObstacle1->GetCollider()->UpdateWorld();

	mObstacle2 = new ModelObject("StanfordBunny/StanfordBunny", Collider::BOX);
	mObstacle2->mScale = { 0.1f, 0.1f, 0.1f };
	mObstacle2->mPosition = { 90, 20, 90 };
	mObstacle2->UpdateWorld();
	mObstacle2->GetCollider()->UpdateWorld();
	

	monsters[0]->mPosition = { 50.0f, 0.0f, 50.0f };
	//monsters[1]->position = { 45.0f, 0.0f, 50.0f };

	monsters[0]->SetTerrain(mTerrain);
	//monsters[1]->SetTerrain(terrain);

	vector<Collider*> monsters0Obstacles;
	//monsters0Obstacles.push_back(mObstacle1->GetCollider());
	//monsters0Obstacles.push_back(mObstacle2->GetCollider());
	//vector<Collider*> monsters1Obstacles;

	//monsters0Obstacles.push_back(monsters[1]->GetColliderForAStar());
	//monsters1Obstacles.push_back(monsters[0]->GetColliderForAStar());

	monsters[0]->GetAStar()->SetObstacle(monsters0Obstacles);
	//monsters[1]->GetAStar()->SetObstacle(monsters1Obstacles);


	for (int y = -3; y < 4; y++)
	{
		for (int i = 0; i < 25; i++)
		{
			for (int j = 0; j < 25; j++)
			{
				cube = new Cube();
				cube->mPosition.x = i * 10.0f;
				cube->mPosition.y = y * 10.0f;
				cube->mPosition.z = j * 10.0f;
				cubes.push_back(cube);
			}
		}
	}
	
	
	mTargetCameraFrustum = Environment::Get()->GetTargetCamera()->GetFrustum();
}

MainScene::~MainScene()
{
	delete mTerrain;
}

void MainScene::Update()
{
	mTerrain->Update();
	mPlayer->Update();
	
	for (int i = 0; i < monsters.size(); i++)
	{
		monsters[i]->Update();
	}

	//for (int i = 0; i < cubes.size(); i++)
	//{
	//	if (mTargetCameraFrustum->ContainPoint(cubes[i]->mPosition))
	//	{
	//		cubes[i]->Update();
	//	}
	//}
}

void MainScene::PreRender()
{
}

void MainScene::Render()
{
	mTerrain->Render();
	mPlayer->Render();

	for (int i = 0; i < monsters.size(); i++)
	{
		monsters[i]->Render();
	}

	/*mObstacle1->Render();
	mObstacle2->Render();*/

	//monsters[0]->GetAStar()->Render();
	
	/*for (int i = 0; i < cubes.size(); i++)
	{
		if (mTargetCameraFrustum->ContainPoint(cubes[i]->mPosition))
		{
			cubes[i]->Render();
		}
	}*/
}

void MainScene::PostRender()
{
	mPlayer->PostRender();
}
