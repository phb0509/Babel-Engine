#include "Framework.h"
#include "GameObject/Utility/E_States.h"
#include "MainScene.h"

MainScene::MainScene() :
	mTargetCameraFrustum(nullptr),
	mbIsWorldMode(true),
	mMainCamera(eCamera::WorldCamera),
	mPreFrameMousePosition(MOUSEPOS),
	mbIsInstancingMode(false)
{
	mWorldCamera = new Camera();
	mWorldCamera->mTag = "WorldCamera";
	mWorldCamera->mPosition = { 72.8f, 73.5f, -93.0f };
	mWorldCamera->mRotation = { 0.5f, -0.3f, 0.0f };

	mTargetCamera = new Camera();
	mTargetCamera->mTag = "TargetCamera";
	mTargetCamera->SetProjectionOption(XM_PIDIV4, WIN_WIDTH / (float)WIN_HEIGHT, 0.5f, 200.0f);
	mTargetCamera->SetIsUsingFrustumCulling(true);
	mTargetCamera->SetIsRenderFrustumCollider(true);

	mTargetCameraForShow = new Camera();
	mTargetCameraForShow->mTag = "TargetCameraInWorld";
	mTargetCameraForShow->SetProjectionOption(XM_PIDIV4, WIN_WIDTH / (float)WIN_HEIGHT, 0.5f, 200.0f);
	mTargetCameraForShow->SetIsUsingFrustumCulling(true);
	mTargetCameraForShow->SetIsRenderFrustumCollider(true);

	mTerrain = new Terrain();
	mTerrain->SetCamera(mWorldCamera);
	mPlayer = GM->GetPlayer();
	mPlayer->SetTerrain(mTerrain);
	mPlayer->SetTargetCamera(mTargetCamera);
	mPlayer->SetTargetCameraInWorld(mTargetCameraForShow);
	mPlayer->SetIsTargetMode(false);
	vector<Collider*> monsters0Obstacles = {};

	float startX = 100.0f;
	float startZ = 100.0f;
	float gapWidth = 10.0f;
	float gapHeight = 10.0f;

	mbIsInstancingMode = true;

	if (mbIsInstancingMode)
	{
		int row = 20;
		int column = 20;

		mMutantInstanceCount = row * column;

		mInstancingMutants = new InstancingMutants(mMutantInstanceCount, mTerrain); // ModelAnimators
		mInstancingMutants->SetCameraForCulling(mTargetCameraForShow); // Default는 일단 WorldMode.
		mInstancingMutants->SetIsFrustumCullingMode(true);
		mInstanceMutants = mInstancingMutants->GetInstanceObjects(); // InstanceObjectsVector

		for (int z = 0; z < column; z++)
		{
			for (int x = 0; x < row; x++)
			{
				int monsterIndex = z * row + x;
				mInstanceMutants[monsterIndex]->SetInstanceIndex(monsterIndex);
				mInstanceMutants[monsterIndex]->mPosition.x = startX + gapWidth * x;
				mInstanceMutants[monsterIndex]->mPosition.z = startZ + gapHeight * z;
				mInstanceMutants[monsterIndex]->SetTerrain(mTerrain, false);
				mInstanceMutants[monsterIndex]->GetAStar()->SetObstacle(monsters0Obstacles);
			}
		}
	}

	else
	{
		int tempRow = 2;
		int tempColumn = 2;

		for (int i = 0; i < tempRow * tempColumn; i++)
		{
			Monster* temp = new Mutant;
			mMutants.push_back(temp);
		}

		for (int z = 0; z < tempColumn; z++)
		{
			for (int x = 0; x < tempRow; x++)
			{
				int monsterIndex = z * tempRow + x;
				mMutants[monsterIndex]->mTag = to_string(monsterIndex) + "번 몬스터";
				mMutants[monsterIndex]->mPosition.x = startX + gapWidth * x;
				mMutants[monsterIndex]->mPosition.z = startZ + gapHeight * z;
				mMutants[monsterIndex]->SetTerrain(mTerrain, false);
				mMutants[monsterIndex]->GetAStar()->SetObstacle(monsters0Obstacles);
			}
		}
	}
}

MainScene::~MainScene()
{
	delete mTerrain;

	for (int i = 0; i < mMutants.size(); i++)
	{
		delete mMutants[i];
		mMutants[i] = nullptr;
	}
}

void MainScene::Update()
{
	switch (static_cast<int>(mMainCamera))
	{
	case 0:    // World
	{
		mWorldCamera->Update();
		moveWorldCamera();
	}
	break;

	case 1:    // Target
	{
		mTargetCamera->Update();
	}
	break;

	default:
		break;
	}

	mTerrain->Update();
	mPlayer->Update(); // Update TargetCameraInWorld

	if (mbIsInstancingMode)
	{
		mInstancingMutants->Update();
	}
	else
	{
		for (int i = 0; i < mMutants.size(); i++)
		{
			mMutants[i]->Update();
		}
	}
}

void MainScene::PreRender()
{

}

void MainScene::Render()
{
	// 백버퍼와 연결된 렌더타겟
	Device::Get()->ClearRenderTargetView(Float4(0.18f, 0.18f, 0.25f, 1.0f));
	Device::Get()->ClearDepthStencilView();
	Device::Get()->SetRenderTarget();
	Environment::Get()->Set(); // SetViewPort

	switch (static_cast<int>(mMainCamera)) // 메인백버퍼에 렌더할 카메라.
	{
	case 0:    // World
	{
		mWorldCamera->SetViewBuffer();
		mWorldCamera->SetProjectionBuffer();
		mTargetCameraForShow->RenderFrustumCollider();
	}
	break;

	case 1:    // Target
	{
		mTargetCamera->SetViewBuffer();
		mTargetCamera->SetProjectionBuffer();
		//mTargetCamera->RenderFrustumCollider();
	}
	break;

	default:
		break;
	}

	mTerrain->Render();
	mPlayer->Render();

	if (mbIsInstancingMode)
	{
		mInstancingMutants->Render();
	}
	else
	{
		for (int i = 0; i < mMutants.size(); i++)
		{
			mMutants[i]->Render();
		}
	}
}

void MainScene::PostRender()
{
	mPlayer->PostRender();

	if (mbIsInstancingMode)
	{
		mInstancingMutants->PostRender();
	}

	ImGui::Begin("Camera Info");
	ImGui::Text("SelectCamera");

	if (ImGui::Button("TargetCamera")) // 
	{
		mMainCamera = eCamera::TargetCamera;
		mPlayer->SetIsTargetMode(true);
		mTargetCameraForShow->SetIsUsingFrustumCulling(false);
		mTargetCamera->SetIsUsingFrustumCulling(true);
		mInstancingMutants->SetCameraForCulling(mTargetCamera);
	}

	if (ImGui::Button("WorldCamera"))
	{
		mMainCamera = eCamera::WorldCamera;
		mPlayer->SetIsTargetMode(false);
		mTargetCameraForShow->SetIsUsingFrustumCulling(true);
		mTargetCamera->SetIsUsingFrustumCulling(false);
		mInstancingMutants->SetCameraForCulling(mTargetCameraForShow);
	}

	SpacingRepeatedly(2);
	ImGui::Text("WorldCameraPosition : %.1f,  %.1f,  %.1f", mWorldCamera->mPosition.x, mWorldCamera->mPosition.y, mWorldCamera->mPosition.z);
	SpacingRepeatedly(2);
	ImGui::Text("MousePosition : %d, %d", (int)MOUSEPOS.x, (int)MOUSEPOS.y);
	SpacingRepeatedly(2);
	ImGui::InputFloat3("Player Position", (float*)&mPlayer->mPosition, "%.3f");
	SpacingRepeatedly(2);

	ImGui::End();
}

void MainScene::printToCSV() // 트랜스폼값같은거 csv로 편하게 볼려고 저장하는 함수.
{
	FILE* file;
	fopen_s(&file, "TextData/monstersPosition.csv", "w");

	for (UINT i = 0; i < mMutants.size(); i++)
	{
		fprintf(
			file,
			"%d,%.3f,%.3f,%.3f\n",
			i,
			mMutants[i]->mPosition.x, mMutants[i]->mPosition.y, mMutants[i]->mPosition.z
		);
	}

	fclose(file);
}

void MainScene::moveWorldCamera()
{
	// Update Position
	if (KEY_PRESS(VK_RBUTTON))
	{
		if (KEY_PRESS('I'))
			mWorldCamera->mPosition += mWorldCamera->Forward() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('K'))
			mWorldCamera->mPosition -= mWorldCamera->Forward() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('J'))
			mWorldCamera->mPosition -= mWorldCamera->Right() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('L'))
			mWorldCamera->mPosition += mWorldCamera->Right() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('U'))
			mWorldCamera->mPosition -= mWorldCamera->Up() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('O'))
			mWorldCamera->mPosition += mWorldCamera->Up() * mWorldCamera->mMoveSpeed * DELTA;
	}

	mWorldCamera->mPosition += mWorldCamera->Forward() * Control::Get()->GetWheel() * mWorldCamera->mWheelSpeed * DELTA;

	// Update Rotation
	if (KEY_PRESS(VK_RBUTTON))
	{
		Vector3 value = MOUSEPOS - mPreFrameMousePosition;

		mWorldCamera->mRotation.x += value.y * mWorldCamera->mRotationSpeed * DELTA;
		mWorldCamera->mRotation.y += value.x * mWorldCamera->mRotationSpeed * DELTA;
	}

	mPreFrameMousePosition = MOUSEPOS;
	mWorldCamera->SetViewMatrixToBuffer();
}




