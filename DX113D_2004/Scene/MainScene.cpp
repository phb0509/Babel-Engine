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
	monsters0Obstacles.push_back(mObstacle1->GetCollider());
	monsters0Obstacles.push_back(mObstacle2->GetCollider());
	//vector<Collider*> monsters1Obstacles;

	//monsters0Obstacles.push_back(monsters[1]->GetColliderForAStar());
	//monsters1Obstacles.push_back(monsters[0]->GetColliderForAStar());

	monsters[0]->GetAStar()->SetObstacle(monsters0Obstacles);
	//monsters[1]->GetAStar()->SetObstacle(monsters1Obstacles);


	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 25; j++)
		{
			cube = new Cube();
			cube->mPosition.x = i * 10.0f;
			cube->mPosition.z = j * 10.0f;
			cubes.push_back(cube);
		}
	}

	mTargetCameraFrustum = Environment::Get()->GetTargetCamera()->GetFrustum();
	testCube = new Cube();
	testCube->SetParent(mPlayer->GetWorld());
	testCube->mPosition.x += 20.0f;
	testCube->mScale = { 50.0f, 50.0f, 50.0f };
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

	for (int i = 0; i < cubes.size(); i++)
	{
		cubes[i]->Update();
		//cubes[i]->position.y = mTerrain->GetHeight(cubes[i]->position);
		cubes[i]->mPosition.y = 20.0f;
	}

	testCube->Update();
	
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

	mObstacle1->Render();
	mObstacle2->Render();

	//monsters[0]->GetAStar()->Render();
	
	for (int i = 0; i < cubes.size(); i++)
	{
		if (mTargetCameraFrustum->ContainPoint(cubes[i]->mPosition))
		{
			cubes[i]->Render();
		}
	}
	
	testCube->Render();
}

void MainScene::PostRender()
{
	mPlayer->PostRender();
	ImGui::Text("mPathSize : %d", monsters[0]->GetPath().size());

	igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", 0, ".");
	igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");

	string tc = "TestCube";
	ImGui::InputFloat3(tc.c_str(), (float*)&testCube->mPosition);

}
