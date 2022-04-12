#include "Framework.h"
#include "MapToolScene.h"

MapToolScene::MapToolScene()
{
	mCamera->mPosition = { -35.2f,113.6f, -105.7f };
	mCamera->mRotation = { 0.5f,0.4f, 0.0f };
	mCamera->mMoveSpeed = 200.0f;
	mCamera->mWheelSpeed = 30.0f;

	mTerrainEditor = new TerrainEditor();
	//skyBox = new SkyBox();

	mRasterizerState = new RasterizerState();
	mRasterizerState->FillMode(D3D11_FILL_WIREFRAME);
}

MapToolScene::~MapToolScene()
{
	delete mTerrainEditor;
	delete mRasterizerState;
	//delete skyBox;
}

void MapToolScene::Update()
{
	mCamera->Update();

	mTerrainEditor->Update();
}

void MapToolScene::PreRender()
{
	Environment::Get()->Set(); // ºä¹öÆÛ Set VS
	mCamera->SetViewBuffer();
	mCamera->GetProjectionBufferInUse();

	mTerrainEditor->PreRender();
}

void MapToolScene::Render()
{
	// ¹é¹öÆÛ¿Í ¿¬°áµÈ ·»´õÅ¸°Ù
	Device::Get()->ClearRenderTargetView(Float4(0.18f, 0.18f, 0.25f, 1.0f));
	Device::Get()->ClearDepthStencilView();
	Device::Get()->SetRenderTarget();

	Environment::Get()->Set(); // SetViewPort
	mCamera->SetViewBuffer();
	mCamera->GetProjectionBufferInUse();


	//skyBox->Render();
	//mRasterizerState->SetState();
	mTerrainEditor->Render();
}

void MapToolScene::PostRender()
{
	mTerrainEditor->PostRender();
	//ImGui::ShowDemoWindow();
	//ImGui::ShowMetricsWindow();
}
