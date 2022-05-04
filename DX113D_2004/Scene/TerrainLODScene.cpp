#include "Framework.h"
#include "TerrainLODScene.h"

TerrainLODScene::TerrainLODScene()
{
	mLightBuffer = new LightBuffer();
	mDirectionalLight = new Light();
	mLightBuffer->Add(mDirectionalLight);
	mCamera = new Camera();
	mCamera->mPosition = { 170.0f, 73.0f, -9.2f };
	mCamera->mRotation = { 0.7f, -1.7f, 0.0f };
	mCamera->SetIsUsingFrustumCulling(true);
	
	
	mTerrain = new TerrainLOD(L"Textures/HeightMap.png");
	mTerrain->SetCamera(mCamera);

	mRSState = new RasterizerState();
	mRSState->FillMode(D3D11_FILL_WIREFRAME);
}

TerrainLODScene::~TerrainLODScene()
{
	delete mTerrain;
	delete mRSState;
}

void TerrainLODScene::Update()
{
	mLightBuffer->Update();
	mDirectionalLight->Update();

	mCamera->Update();
	mCamera->Move();
	mTerrain->Update();
}

void TerrainLODScene::PreRender()
{
	
}

void TerrainLODScene::Render()
{
	Device::Get()->ClearRenderTargetView(Float4(0.18f, 0.18f, 0.25f, 1.0f));
	Device::Get()->ClearDepthStencilView();
	Device::Get()->SetRenderTarget();
	Environment::Get()->Set(); //
	mCamera->SetViewBufferToVS();
	mCamera->SetProjectionBufferToVS();


	mRSState->SetState();
	mTerrain->Render();
}

void TerrainLODScene::PostRender()
{
	mTerrain->PostRender();
	mLightBuffer->PostRender();
	mDirectionalLight->PostRender();

	SpacingRepeatedly(2);
	ImGui::Text("WorldCameraPosition : %.1f,  %.1f,  %.1f", mCamera->mPosition.x, mCamera->mPosition.y, mCamera->mPosition.z);
	ImGui::Text("WorldCameraPosition : %.1f,  %.1f,  %.1f", mCamera->mRotation.x, mCamera->mRotation.y, mCamera->mRotation.z);
	SpacingRepeatedly(2);
}
