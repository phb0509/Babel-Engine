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
	mDeferredMaterial = new Material(L"DeferredLighting");
	UINT vertices[4] = { 0, 1, 2, 3 };
	mVertexBuffer = new VertexBuffer(vertices, sizeof(UINT), 4);
	mGBuffer = new GBuffer();

	mLightBuffer = new LightBuffer();
	mDirectionalLight = new Light(LightType::DIRECTIONAL);
	mDirectionalLight->SetTag("Directional Light1");
	mLightBuffer->Add(mDirectionalLight);

	mWorldCamera = new Camera();
	mWorldCamera->SetTag("WorldCamera");
	mWorldCamera->mPosition = { 72.8f, 73.5f, -93.0f };
	mWorldCamera->mRotation = { 0.5f, -0.3f, 0.0f };

	mTargetCamera = new Camera();
	mTargetCamera->SetTag("TargetCamera");
	mTargetCamera->SetProjectionOption(XM_PIDIV4, WIN_WIDTH / (float)WIN_HEIGHT, 0.5f, 200.0f);
	mTargetCamera->SetIsUsingFrustumCulling(true);
	mTargetCamera->SetIsRenderFrustumCollider(true);

	mTargetCameraForShow = new Camera();
	mTargetCameraForShow->SetTag("TargetCameraInWorld");
	mTargetCameraForShow->SetProjectionOption(XM_PIDIV4, WIN_WIDTH / (float)WIN_HEIGHT, 0.5f, 200.0f);
	mTargetCameraForShow->SetIsUsingFrustumCulling(true);
	mTargetCameraForShow->SetIsRenderFrustumCollider(true);

	mTerrain = new Terrain();
	mTerrain->SetCamera(mWorldCamera);
	mTerrain->GetMaterial()->SetShader(L"GBuffer");

	mPlayer = GM->GetPlayer();
	mPlayer->SetTerrain(mTerrain);
	mPlayer->SetTargetCamera(mTargetCamera);
	mPlayer->SetTargetCameraInWorld(mTargetCameraForShow);
	mPlayer->SetIsTargetMode(false);
	mPlayer->SetShader(L"GBuffer");

	vector<Collider*> monsters0Obstacles = {};

	float startX = 100.0f;
	float startZ = 100.0f;
	float gapWidth = 10.0f;
	float gapHeight = 10.0f;

	int row = 5;
	int column = 5;

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

	mPlayer->SetMonsters("Mutant", mInstanceMutants);
}

MainScene::~MainScene()
{
	GM->SafeDelete(mTerrain);
	GM->SafeDeleteVector(mMutants);
	GM->SafeDelete(mWorldCamera);
	GM->SafeDelete(mTargetCamera);
	GM->SafeDelete(mTargetCameraForShow);
}

void MainScene::Update()
{
	mLightBuffer->Update();
	mDirectionalLight->Update();

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
	mInstancingMutants->Update();
	executeEvent();

	DM->Update();
}

void MainScene::PreRender()
{
	Environment::Get()->Set(); // SetViewPort
	mGBuffer->PreRender();

	switch (static_cast<int>(mMainCamera)) // 메인백버퍼에 렌더할 카메라.
	{
	case 0:    // World
	{
		mWorldCamera->SetViewBufferToVS();
		mWorldCamera->SetProjectionBufferToVS();
		mTargetCameraForShow->RenderFrustumCollider();
	}
	break;

	case 1:    // Target
	{
		mTargetCamera->SetViewBufferToVS();
		mTargetCamera->SetProjectionBufferToVS();
	}
	break;

	default:
		break;
	}

	mTerrain->GetMaterial()->SetShader(L"GBuffer");
	mPlayer->SetShader(L"GBuffer");
	mInstancingMutants->SetShader(L"InstancingMutantsGBuffer");
	mDirectionalLight->GetSphere()->GetMaterial()->SetShader(L"GBuffer");

	mTerrain->Render();
	mPlayer->DeferredRender();
	mInstancingMutants->Render();
	//mDirectionalLight->Render();
}

void MainScene::Render()
{
	// 백버퍼와 연결된 렌더타겟
	Device::Get()->ClearRenderTargetView(Float4(0.18f, 0.18f, 0.25f, 1.0f));
	Device::Get()->ClearDepthStencilView();
	Device::Get()->SetRenderTarget();
	Environment::Get()->Set(); // SetViewPort
	mLightBuffer->SetPSBuffer(0);

	switch (static_cast<int>(mMainCamera)) // 메인백버퍼에 렌더할 카메라.
	{
	case 0:    // World
	{
		mWorldCamera->SetViewBufferToVS(10);
		mWorldCamera->SetProjectionBufferToVS(11);
		//mTargetCameraForShow->RenderFrustumCollider();
	}
	break;

	case 1:    // Target
	{
		mTargetCamera->SetViewBufferToVS(10);
		mTargetCamera->SetProjectionBufferToVS(11);
	}
	break;

	default:
		break;
	}

	mGBuffer->SetRenderTargetsToPS();

	mVertexBuffer->SetIA(); // 디폴트 0번.
	DEVICECONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	mDeferredMaterial->Set(); // 디퍼드라이팅셰이더파일 Set. 이거 Set하고 Draw했으니 딱 맞다.
	DEVICECONTEXT->Draw(4, 0);

	mGBuffer->ClearSRVs();
}

void MainScene::PostRender()
{
	mPlayer->PostRender();
	mLightBuffer->PostRender();
	mDirectionalLight->PostRender();
	mGBuffer->PostRender();
	mInstancingMutants->PostRender();

	if (mbIsInstancingMode)
	{
		mInstancingMutants->PostRender();
	}

	ImGui::Begin("Camera Info");
	ImGui::Text("SelectCamera");

	if (ImGui::Button("TargetCamera"))
	{
		mMainCamera = eCamera::TargetCamera;
		mPlayer->SetIsTargetMode(true);
		mTargetCameraForShow->SetIsUsingFrustumCulling(false);
		mTargetCamera->SetIsUsingFrustumCulling(true);
		mInstancingMutants->SetCameraForCulling(mTargetCamera);
		mInstancingMutants->SetCurMainCamaera(mTargetCamera);
	}

	if (ImGui::Button("WorldCamera"))
	{
		mMainCamera = eCamera::WorldCamera;
		mPlayer->SetIsTargetMode(false);
		mTargetCameraForShow->SetIsUsingFrustumCulling(true);
		mTargetCamera->SetIsUsingFrustumCulling(false);
		mInstancingMutants->SetCameraForCulling(mTargetCameraForShow);
		mInstancingMutants->SetCurMainCamaera(mWorldCamera);
	}

	SpacingRepeatedly(2);
	mWorldCamera->PostTransformRender();
	SpacingRepeatedly(2);
	ImGui::Text("MousePosition : %d, %d", (int)MOUSEPOS.x, (int)MOUSEPOS.y);
	SpacingRepeatedly(2);
	mPlayer->PostTransformRender();
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
			mWorldCamera->mPosition += mWorldCamera->GetForwardVector() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('K'))
			mWorldCamera->mPosition -= mWorldCamera->GetForwardVector() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('J'))
			mWorldCamera->mPosition -= mWorldCamera->GetRightVector() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('L'))
			mWorldCamera->mPosition += mWorldCamera->GetRightVector() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('U'))
			mWorldCamera->mPosition -= mWorldCamera->GetUpVector() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('O'))
			mWorldCamera->mPosition += mWorldCamera->GetUpVector() * mWorldCamera->mMoveSpeed * DELTA;
	}

	mWorldCamera->mPosition += mWorldCamera->GetForwardVector() * Control::Get()->GetWheel() * mWorldCamera->mWheelSpeed * DELTA;

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

void MainScene::executeEvent()
{
	respawnMonster();
}

void MainScene::respawnMonster()
{
	for (int i = 0; i < mInstanceMutants.size(); i++)
	{
		Monster* monster = mInstanceMutants[i];

		if (!monster->GetIsActive())
		{
			if (TIME - monster->GetLastTimeDeactivation() >= 5.0f) // 죽은지 1초 지났으면, 재활성화
			{
				monster->mPosition = monster->GetLastPosDeactivation();
				monster->ReActivation();
			}
		}
	}
}




