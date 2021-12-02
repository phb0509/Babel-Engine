#include "Framework.h"
#include "MainScene.h"

MainScene::MainScene() :
	mTargetCameraFrustum(nullptr)
{
	mTerrain = new Terrain();
	mPlayer = GM->GetPlayer();
	mPlayer->SetTerrain(mTerrain);
	Environment::Get()->SetTargetToCamera(mPlayer);
	monsters = GM->GetMonsters();

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
	//		transform->tag = "x : " + to_string((int)x) + " z : " + to_string((int)z);
	//		transform->mPosition = { x, 0.0f, z };
	//		transform->mScale = { 0.1f, 0.1f, 0.1f };

	//		mInstancedMutants->SetEndEvents(boneMatrix.size(), 2, // �ν��Ͻ�,Ŭ��
	//			bind(&MainScene::SetIdle, this, placeholders::_1));
	//		mInstancedMutants->SetParams(boneMatrix.size(), 2, boneMatrix.size());

	//		boneMatrix.emplace_back(XMMatrixIdentity()); // �ν��Ͻ�����ŭ ������ķ� �ʱ�ȭ.			
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

	for (int i = 0; i < monsters.size(); i++)
	{
		monsters[i]->Update();
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

	for (int i = 0; i < monsters.size(); i++)
	{
		monsters[i]->Render();
	}

	//mInstancedMutants->Render();
}

void MainScene::PostRender()
{
	mPlayer->PostRender();
}



void MainScene::printToCSV() // Ʈ�������������� csv�� ���ϰ� ������ �����ϴ� �Լ�.
{
	FILE* file;
	fopen_s(&file, "TextData/monstersPosition.csv", "w");

	for (UINT i = 0; i < monsters.size(); i++)
	{
		fprintf(
			file,
			"%d,%.3f,%.3f,%.3f\n",
			i,
			monsters[i]->mPosition.x, monsters[i]->mPosition.y, monsters[i]->mPosition.z
		);
	}

	fclose(file);
}

void MainScene::setIdle(int instance)
{
	mInstancedMutants->PlayClip(instance, 0); // Idle.
}
