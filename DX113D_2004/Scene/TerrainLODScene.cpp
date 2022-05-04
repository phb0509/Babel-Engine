#include "Framework.h"
#include "TerrainLODScene.h"

TerrainLODScene::TerrainLODScene()
{
	mLightBuffer = new LightBuffer();
	mDirectionalLight = new Light();
	mLightBuffer->Add(mDirectionalLight);

	mCamera = new Camera();
	mCamera->mPosition = { 7.5f, 117.3f, -81.2f };
	mCamera->mRotation = { 0.8f, 0.0f, 0.0f };
	mCamera->SetIsUsingFrustumCulling(true);
	mCamera->mMoveSpeed = 200.0f;
	
	
	mLODTerrain = new TerrainLOD(L"Textures/HeightMap.png");
	mLODTerrain->SetCamera(mCamera);

	mRSState = new RasterizerState();
	mRSState->FillMode(D3D11_FILL_WIREFRAME);

	//mPlayer = new ModelAnimObject();
	//mPlayer->mScale = { 0.05f, 0.05f, 0.05f };
	////mPlayer->mPosition.x = 0.0f;
	//mPlayer->SetShader(L"ModelAnimation");
	//mPlayer->SetMesh("Player", "Player.mesh");
	//mPlayer->SetMaterial("Player", "Player.mat");
	///*mPlayer->SetDiffuseMap(L"ModelData/Player/Paladin_diffuse.png");
	//mPlayer->SetNormalMap(L"ModelData/Player/Paladin_normal.png");
	//mPlayer->SetSpecularMap(L"ModelData/Player/Paladin_specular.png");*/
	//mPlayer->ReadClip("Player", "Idle.clip");
	//mPlayer->mPosition = { 0.0f,0.0f,0.0f };
	//mPlayer->UpdateWorld();

	mPlayer = GM->GetPlayer();
	mPlayer->SetLODTerrain(mLODTerrain);
	//mPlayer->SetTargetCamera(mTargetCamera);
	mPlayer->SetTargetCameraInWorld(mCamera);
	mPlayer->SetIsTargetMode(false);
	mPlayer->SetShader(L"ModelAnimation");

	mSphere = new Sphere(L"Lighting");
	mSphere->mScale = { 0.3f, 0.3f, 0.3f };

}

TerrainLODScene::~TerrainLODScene()
{
	delete mLODTerrain;
	delete mRSState;
}

void TerrainLODScene::Update()
{
	mLightBuffer->Update();
	mDirectionalLight->Update();

	mCamera->Update();
	mCamera->Move();
	mLODTerrain->Update();
	mSphere->Update();
	//mPlayer->Update();
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
	mLODTerrain->Render();
	//mPlayer->Render();
	mSphere->Render();
}

void TerrainLODScene::PostRender()
{
	mLODTerrain->PostRender();
	mLightBuffer->PostRender();
	mDirectionalLight->PostRender();

	SpacingRepeatedly(2);
	ImGui::Text("WorldCameraPosition : %.1f,  %.1f,  %.1f", mCamera->mPosition.x, mCamera->mPosition.y, mCamera->mPosition.z);
	ImGui::Text("WorldCameraPosition : %.1f,  %.1f,  %.1f", mCamera->mRotation.x, mCamera->mRotation.y, mCamera->mRotation.z);
	SpacingRepeatedly(2);


	SpacingRepeatedly(2);
	ImGui::InputFloat3("Sphere Position", (float*)&mSphere->mPosition, "%.3f");
	SpacingRepeatedly(2);
}
