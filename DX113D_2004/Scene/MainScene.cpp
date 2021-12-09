#include "Framework.h"
#include "MainScene.h"

MainScene::MainScene() :
	mTargetCameraFrustum(nullptr)
{
	mTerrain = new Terrain();
	mPlayer = GM->GetPlayer();
	mPlayer->SetTerrain(mTerrain);
	Environment::Get()->SetTargetToCamera(mPlayer);
	mMonsters = GM->GetMonsters();

	/*mObstacle1 = new ModelObject("StanfordBunny/StanfordBunny", Collider::BOX);
	mObstacle1->mScale = { 0.1f, 0.1f, 0.1f };
	mObstacle1->mPosition = { 40, 20, 40 };
	mObstacle1->UpdateWorld();
	mObstacle1->GetCollider()->UpdateWorld();

	mObstacle2 = new ModelObject("StanfordBunny/StanfordBunny", Collider::BOX);
	mObstacle2->mScale = { 0.1f, 0.1f, 0.1f };
	mObstacle2->mPosition = { 90, 20, 90 };
	mObstacle2->UpdateWorld();
	mObstacle2->GetCollider()->UpdateWorld();*/


	mMonsters[0]->mPosition = { 50.0f, 0.0f, 50.0f };
	//mMonsters[1]->position = { 45.0f, 0.0f, 50.0f };

	mMonsters[0]->SetTerrain(mTerrain);
	//mMonsters[1]->SetTerrain(terrain);

	vector<Collider*> monsters0Obstacles;
	//monsters0Obstacles.push_back(mObstacle1->GetCollider());
	//monsters0Obstacles.push_back(mObstacle2->GetCollider());
	//vector<Collider*> monsters1Obstacles;

	//monsters0Obstacles.push_back(mMonsters[1]->GetColliderForAStar());
	//monsters1Obstacles.push_back(mMonsters[0]->GetColliderForAStar());

	mMonsters[0]->GetAStar()->SetObstacle(monsters0Obstacles);
	//mMonsters[1]->GetAStar()->SetObstacle(monsters1Obstacles);

	mTargetCameraFrustum = Environment::Get()->GetTargetCamera()->GetFrustum();
	
	//mInstancedMutants = new ModelAnimators("Mutant/Mutant");
	//mInstancedMutants->SetShader(L"Models");
	//mInstancedMutants->ReadClip("Mutant/Idle0.clip");

	//Vector3 minBox, maxBox;
	//mInstancedMutants->SetBox(&minBox, &maxBox);

	//for (float z = 0.0f; z < 5.0f; z++)
	//{
	//	for (float x = 0.0f; x < 50.0f; x++)
	//	{
	//		Transform* transform = mInstancedMutants->Add();
	//		transform->mTag = "x : " + to_string((int)x) + " z : " + to_string((int)z);
	//		transform->mPosition = { x, 0.0f, z };
	//		transform->mScale = { 0.1f, 0.1f, 0.1f };

	//		mInstancedMutants->SetEndEvents(boneMatrix.size(), 2, // 인스턴스,클립
	//			bind(&MainScene::SetIdle, this, placeholders::_1));
	//		mInstancedMutants->SetParams(boneMatrix.size(), 2, boneMatrix.size());

	//		boneMatrix.emplace_back(XMMatrixIdentity()); // 인스턴스수만큼 단위행렬로 초기화.			
	//	}
	//}
}

MainScene::~MainScene()
{
	delete mTerrain;
}

void MainScene::Update()
{
	mTerrain->Update();
	mPlayer->Update();

	for (int i = 0; i < mMonsters.size(); i++)
	{
		mMonsters[i]->Update();
	}

	//mInstancedMutants->Update();
}

void MainScene::PreRender()
{
}

void MainScene::Render()
{
	mTerrain->Render();
	mPlayer->Render();

	for (int i = 0; i < mMonsters.size(); i++)
	{
		mMonsters[i]->Render();
	}

	//mInstancedMutants->Render();
}

void MainScene::PostRender()
{
	mPlayer->PostRender();
}



void MainScene::printToCSV() // 트랜스폼값같은거 csv로 편하게 볼려고 저장하는 함수.
{
	FILE* file;
	fopen_s(&file, "TextData/monstersPosition.csv", "w");

	for (UINT i = 0; i < mMonsters.size(); i++)
	{
		fprintf(
			file,
			"%d,%.3f,%.3f,%.3f\n",
			i,
			mMonsters[i]->mPosition.x, mMonsters[i]->mPosition.y, mMonsters[i]->mPosition.z
		);
	}

	fclose(file);
}

void MainScene::setIdle(int instance)
{
	mInstancedMutants->PlayClip(instance, 0); // Idle.
}
