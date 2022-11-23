#include "Framework.h"
#include "GameObject/Utility/E_States.h"
#include "MainScene.h"

MainScene::MainScene() :
	mbIsWorldMode(true),
	mMainCamera(eCamera::WorldCamera),
	mPreFrameMousePosition(MOUSEPOS),
	mbIsInstancingMode(false)
{
	initDeferred();
	initLight();
	initCamera();
	initTerrain();
	initPlayer();
	initInstances();
	initShadowMapping();
}

MainScene::~MainScene()
{
	GM->SafeDelete(mTerrain);
	GM->SafeDelete(mWorldCamera);
	GM->SafeDelete(mTargetCamera);
	GM->SafeDelete(mTargetCameraForShow);
	GM->SafeDelete(mLightBuffer);
	GM->SafeDelete(mDirectionalLight);
	GM->SafeDelete(mGBuffer);
	GM->SafeDelete(mDeferredMaterial);
	GM->SafeDelete(mDeferredVertexBuffer);
	GM->SafeDelete(mDirectionalLightDepthMapForShow);
	GM->SafeDelete(mDirectionalLightDSV);
	GM->SafeDelete(mShadowMappingLightBuffer);
}

void MainScene::Update()
{
	updateLight();
	updateCamera();

	mTerrain->Update();
	mPlayer->Update(); // Update TargetCameraInWorld 
	mInstancingMutants->Update();
	executeEvent();

	DM->Update();
}

void MainScene::PreRender()
{
	renderShadowDepth();
	renderGBuffer(); 
}

void MainScene::Render()
{
	deferredRender();
}

void MainScene::PostRender()
{
	Environment::Get()->SetViewport();

	mLightBuffer->PostRender();
	mDirectionalLight->PostRender();
	mGBuffer->PostRender();

	mPlayer->PostRender();
	mPlayer->UIRender();

	mInstancingMutants->PostRender();
	mInstancingMutants->UIRender();

	showCameraInfo();
	showShadowDepthMap();
}


void MainScene::updateLight()
{
	mLightBuffer->Update();
	mDirectionalLight->Update();
	moveLight();
}

void MainScene::moveLight()
{
	if (KEY_PRESS('T'))
		mDirectionalLight->mPosition += mDirectionalLight->GetForwardVector() * mWorldCamera->mMoveSpeed * DELTA;
	if (KEY_PRESS('G'))
		mDirectionalLight->mPosition -= mDirectionalLight->GetForwardVector() * mWorldCamera->mMoveSpeed * DELTA;
	if (KEY_PRESS('F'))
		mDirectionalLight->mPosition -= mDirectionalLight->GetRightVector() * mWorldCamera->mMoveSpeed * DELTA;
	if (KEY_PRESS('H'))
		mDirectionalLight->mPosition += mDirectionalLight->GetRightVector() * mWorldCamera->mMoveSpeed * DELTA;
}

void MainScene::updateCamera()
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

void MainScene::deferredRender()
{
	Environment::Get()->SetViewport();
	Device::Get()->ClearRenderTargetView(Float4(0.18f, 0.18f, 0.25f, 1.0f)); // 백버퍼와 연결된 렌더타겟을 클리어.
	Device::Get()->ClearDepthStencilView(); // 백버퍼와 연결된 DSV를 Clear.
	Device::Get()->SetRenderTarget(); // 백버퍼와 연결된 렌더타겟과 DSV를 Set.

	mLightBuffer->SetPSBuffer(0);

	switch (static_cast<int>(mMainCamera)) // 메인백버퍼에 렌더할 카메라.
	{
	case 0:  // World
	{
		mWorldCamera->SetViewBufferToVS(10);
		mWorldCamera->SetProjectionBufferToVS(11);
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

	mGBuffer->SetTexturesToPS(); // Set Textures (Depth, Diffuse, Normal, Specular)
	mDeferredVertexBuffer->SetIA(); // Default 0
	DEVICECONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	mDeferredMaterial->Set(); // Set DeferredShader
	DEVICECONTEXT->Draw(4, 0);
	mGBuffer->ClearSRVs(); // Clear Textures
}

void MainScene::initDeferred()
{
	mDeferredMaterial = new Material(L"DeferredLighting");
	UINT vertices[4] = { 0, 1, 2, 3 };
	mDeferredVertexBuffer = new VertexBuffer(vertices, sizeof(UINT), 4);
	mGBuffer = new GBuffer();
}

void MainScene::initLight()
{
	mLightBuffer = new LightBuffer();
	mDirectionalLight = new Light(LightType::DIRECTIONAL);
	mDirectionalLight->SetTag("Directional Light1");
	mLightBuffer->Add(mDirectionalLight);
}

void MainScene::initCamera()
{
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
}

void MainScene::initTerrain()
{
	mTerrain = new Terrain();
	mTerrain->SetCamera(mWorldCamera);
	mTerrain->GetMaterial()->SetShader(L"GBuffer");
}

void MainScene::initPlayer()
{
	mPlayer = GM->GetPlayer();
	mPlayer->SetTerrain(mTerrain);
	mPlayer->SetTargetCamera(mTargetCamera);
	mPlayer->SetTargetCameraInWorld(mTargetCameraForShow);
	mPlayer->SetIsTargetMode(false);
	mPlayer->SetShader(L"GBuffer");
}

void MainScene::initInstances()
{
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
}

void MainScene::initShadowMapping()
{
	mDepthWidth = WIN_WIDTH * 8;
	mDepthHeight = WIN_HEIGHT * 8;

	mDirectionalLightDepthMapForShow = new RenderTarget(mDepthWidth, mDepthHeight, DXGI_FORMAT_R8G8B8A8_UNORM); // 보여주기용.
	mDirectionalLightDSV = new DepthStencil(mDepthWidth, mDepthHeight, true);
	mDirectionalLight->mPosition = { -30.0f, 100.0f, 350.0f };
	mDirectionalLight->mRotation = { 0.483f, 2.537f, 0.0f };
	mShadowMappingLightBuffer = new ShadowMappingLightBuffer();
}

void MainScene::showCameraInfo()
{
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
}

void MainScene::showShadowDepthMap()
{
	ImGui::Begin("ShadowDepthMap");

	int frame_padding = 0;
	ImVec2 imageButtonSize = ImVec2(700.0f, 700.0f); // 이미지버튼 크기설정.                     
	ImVec2 imageButtonUV0 = ImVec2(0.0f, 0.0f); // 출력할이미지 uv좌표설정.
	ImVec2 imageButtonUV1 = ImVec2(1.0f, 1.0f); // 전체다 출력할거니까 1.
	ImVec4 imageButtonBackGroundColor = ImVec4(0.06f, 0.06f, 0.06f, 0.94f); // ImGuiWindowBackGroundColor.
	ImVec4 imageButtonTintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	ImGui::ImageButton(mDirectionalLightDepthMapForShow->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);
	ImGui::End();
}

void MainScene::renderShadowDepth()
{
	Environment::Get()->SetViewport(mDepthWidth, mDepthHeight);

	//그림자매핑용 깊이맵 얻기위한 DSV셋팅. 
	vector<RenderTarget*> renderTargets;
	renderTargets.push_back(mDirectionalLightDepthMapForShow);
	RenderTarget::ClearAndSetWithDSV(renderTargets.data(), 1, mDirectionalLightDSV);

	// 광원기준 뷰행렬이랑 직교투영행렬 생성 후 셰이더에 Set하고 모든 버텍스들 Draw.
	Vector3 lookat = mDirectionalLight->mPosition + mDirectionalLight->GetForwardVector();
	Vector3 upVec = mDirectionalLight->GetUpVector();
	Matrix tempViewMatrix = XMMatrixLookAtLH(mDirectionalLight->mPosition.data, lookat.data, upVec.data);
	Matrix tempProjMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, WIN_WIDTH / (float)WIN_HEIGHT, 1.0, 1000.0f);

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
}

void MainScene::renderGBuffer()
{
	Environment::Get()->SetViewport();
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




