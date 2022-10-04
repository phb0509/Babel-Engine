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
	//mTargetCamera->SetProjectionOption(XM_PIDIV4, WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

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

	int row = 3;
	int column = 3;

	mMutantInstanceCount = row * column;

	mInstancingMutants = new InstancingMutants(mMutantInstanceCount, 200, 200, mTerrain); // ModelAnimators
	mInstancingMutants->SetCameraForCulling(mTargetCameraForShow); // Default는 일단 WorldMode.
	mInstancingMutants->SetCurMainCamera(mWorldCamera);
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

	mDirectionalLightDepthMapForShow = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM); // 보여주기용.
	mDirectionalLightDSV = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true);
	mDirectionalLight->mPosition = { -30.0f, 100.0f, 350.0f };
	mDirectionalLight->mRotation = { 0.483f, 2.537f, 0.0f };

	mDepthStencilStates[0] = new DepthStencilState();
	mDepthStencilStates[1] = new DepthStencilState();
	mDepthStencilStates[0]->DepthEnable(true);
	mDepthStencilStates[1]->DepthEnable(false);

	mBlendStates[0] = new BlendState();
	mBlendStates[1] = new BlendState();
	mBlendStates[0]->SetAlpha(true);
	mBlendStates[1]->SetAlpha(false);

	mShadowMappingLightBuffer = new ShadowMappingLightBuffer();
	//mDeferredRenderTarget.push_back(new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM));
}

MainScene::~MainScene()
{
	GM->SafeDelete(mTerrain);
	GM->SafeDeleteVector(mMutants);
	GM->SafeDelete(mWorldCamera);
	GM->SafeDelete(mTargetCamera);
	GM->SafeDelete(mTargetCameraForShow);
	GM->SafeDelete(mShadowMappingLightBuffer);
}

void MainScene::Update()
{
	mLightBuffer->Update();
	//mDirectionalLight->mPosition = mWorldCamera->mPosition;
	//mDirectionalLight->mRotation = mWorldCamera->mRotation;
	mDirectionalLight->Update();
	//mLightSphere->Update();

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


	if (KEY_PRESS('T'))
		mDirectionalLight->mPosition += mDirectionalLight->GetForwardVector() * mWorldCamera->mMoveSpeed * DELTA;
	if (KEY_PRESS('G'))
		mDirectionalLight->mPosition -= mDirectionalLight->GetForwardVector() * mWorldCamera->mMoveSpeed * DELTA;
	if (KEY_PRESS('F'))
		mDirectionalLight->mPosition -= mDirectionalLight->GetRightVector() * mWorldCamera->mMoveSpeed * DELTA;
	if (KEY_PRESS('H'))
		mDirectionalLight->mPosition += mDirectionalLight->GetRightVector() * mWorldCamera->mMoveSpeed * DELTA;
}

void MainScene::PreRender()
{
	Environment::Get()->SetViewport(); // SetViewPort

	//그림자매핑용 깊이맵 얻기위한 DSV셋팅. 
	vector<RenderTarget*> renderTargets;
	renderTargets.push_back(mDirectionalLightDepthMapForShow);
	RenderTarget::ClearAndSetWithDSV(renderTargets.data(), 1, mDirectionalLightDSV);

	// 광원기준 뷰행렬이랑 직교투영행렬 생성 후 셰이더에 Set하고 모든 버텍스들 Draw.
	Vector3 lookat = mDirectionalLight->mPosition + mDirectionalLight->GetForwardVector();
	Vector3 upVec = mDirectionalLight->GetUpVector();
	Matrix tempViewMatrix = XMMatrixLookAtLH(mDirectionalLight->mPosition.data, lookat.data, upVec.data);
	//Matrix tempProjMatrix = XMMatrixOrthographicLH(WIN_WIDTH, WIN_HEIGHT, 1.0f, 3000.0f);
	Matrix tempProjMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, WIN_WIDTH / (float)WIN_HEIGHT, 0.1, 1000.0f);

	ViewBuffer tempViewBuffer;
	ProjectionBuffer tempProjectionBuffer;

	tempViewBuffer.SetMatrix(tempViewMatrix);
	tempProjectionBuffer.SetMatrix(tempProjMatrix);
	mShadowMappingLightBuffer->SetData(tempViewMatrix, tempProjMatrix);

	tempViewBuffer.SetVSBuffer(1);
	tempProjectionBuffer.SetVSBuffer(2);

	mTerrain->GetMaterial()->SetShader(L"TestLighting");
	mPlayer->SetShader(L"TestModelAnimation");
	mInstancingMutants->SetShader(L"TestInstancingMutants");

	mTerrain->Render();
	mPlayer->Render();
	mInstancingMutants->Render();



	mGBuffer->ClearAndSetRenderTargets(); // 렌더타겟이랑 DSV 클리어 후, 셋팅. 여기 DSV에 디퍼드깊이값 들어있음.

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

	mShadowMappingLightBuffer->SetVSBuffer(9);
	DEVICECONTEXT->PSSetShaderResources(8, 1, &mDirectionalLightDSV->GetSRV());

	mTerrain->GetMaterial()->SetShader(L"GBuffer");
	mPlayer->SetShader(L"GBuffer");
	mInstancingMutants->SetShader(L"InstancingMutantsGBuffer");
	mDirectionalLight->GetSphere()->GetMaterial()->SetShader(L"GBuffer");

	mTerrain->DeferredRender();
	mPlayer->DeferredRender();
	mInstancingMutants->DeferredRender();
	mDirectionalLight->GetSphere()->Render();

	mPlayer->renderDeferredColliders();
	mInstancingMutants->renderDeferredColliders();
}

void MainScene::Render()
{
	// 백버퍼와 연결된 렌더타겟
	Device::Get()->ClearRenderTargetView(Float4(0.18f, 0.18f, 0.25f, 1.0f)); // 백버퍼와 연결된 렌더타겟을 클리어.
	Device::Get()->ClearDepthStencilView(); // 백버퍼와 연결된 DSV를 Clear.
	Device::Get()->SetRenderTarget(); // 백버퍼와 연결된 렌더타겟과 DSV를 Set.

	//Device::Get()->ClearDepthStencilView(mGBuffer->GetDepthStencil()->GetDSV()); // GBuffer DSV 클리어 (백버퍼랑 연결된거랑 별개임)
	//Device::Get()->SetRenderTarget(mGBuffer->GetDepthStencil()->GetDSV()); // 

	Environment::Get()->SetViewport(); // SetViewPort
	mLightBuffer->SetPSBuffer(0);

	switch (static_cast<int>(mMainCamera)) // 메인백버퍼에 렌더할 카메라.
	{
	case 0:  // World
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

	// Set ShadowMapping Resource
	//mShadowMappingLightBuffer->SetVSBuffer(9);
	//DEVICECONTEXT->PSSetShaderResources(8, 1, &mDirectionalLightDSV->GetSRV());

	mGBuffer->SetTexturesToPS(); // Set Textures (Depth, Diffuse, Normal, Specular)
	mVertexBuffer->SetIA(); // Default 0
	DEVICECONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	mDeferredMaterial->Set(); // Set DeferredShader
	DEVICECONTEXT->Draw(4, 0);
	mGBuffer->ClearSRVs(); // Clear Textures
}

void MainScene::PostRender()
{
	Environment::Get()->SetViewport();

	mPlayer->PostRender();
	mPlayer->UIRender();
	mLightBuffer->PostRender();
	mDirectionalLight->PostRender();

	mInstancingMutants->PostRender();
	mInstancingMutants->UIRender();

	mGBuffer->PostRender();

	ImGui::Begin("Camera Info");
	ImGui::Text("SelectCamera");

	if (ImGui::Button("TargetCamera"))
	{
		mMainCamera = eCamera::TargetCamera;
		mPlayer->SetIsTargetMode(true);
		mTargetCameraForShow->SetIsUsingFrustumCulling(false);
		mTargetCamera->SetIsUsingFrustumCulling(true);
		mInstancingMutants->SetCameraForCulling(mTargetCamera);
		mInstancingMutants->SetCurMainCamera(mTargetCamera);
	}

	if (ImGui::Button("WorldCamera"))
	{
		mMainCamera = eCamera::WorldCamera;
		mPlayer->SetIsTargetMode(false);
		mTargetCameraForShow->SetIsUsingFrustumCulling(true);
		mTargetCamera->SetIsUsingFrustumCulling(false);
		mInstancingMutants->SetCameraForCulling(mTargetCameraForShow);
		mInstancingMutants->SetCurMainCamera(mWorldCamera);
	}

	SpacingRepeatedly(2);
	mWorldCamera->PostTransformRender();
	SpacingRepeatedly(2);
	mTargetCamera->PostTransformRender();
	SpacingRepeatedly(2);
	ImGui::Text("MousePosition : %d, %d", (int)MOUSEPOS.x, (int)MOUSEPOS.y);
	SpacingRepeatedly(2);
	mPlayer->PostTransformRender();
	SpacingRepeatedly(2);

	ImGui::End();

	ImGui::Begin("ShadowDepthMap");

	int frame_padding = 0;
	ImVec2 imageButtonSize = ImVec2(400.0f, 400.0f); // 이미지버튼 크기설정.                     
	ImVec2 imageButtonUV0 = ImVec2(0.0f, 0.0f); // 출력할이미지 uv좌표설정.
	ImVec2 imageButtonUV1 = ImVec2(1.0f, 1.0f); // 전체다 출력할거니까 1.
	ImVec4 imageButtonBackGroundColor = ImVec4(0.06f, 0.06f, 0.06f, 0.94f); // ImGuiWindowBackGroundColor.
	ImVec4 imageButtonTintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	ImGui::ImageButton(mDirectionalLightDepthMapForShow->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);
	//ImGui::ImageButton(mDirectionalLightDSV->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);
	ImGui::End();

	//ImGui::Begin("GBuffer DSV DepthTexture");
	//ImGui::ImageButton(mGBuffer->GetDepthStencil()->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);
	//ImGui::End();
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




