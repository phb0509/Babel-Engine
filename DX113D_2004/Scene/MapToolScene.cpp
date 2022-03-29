#include "Framework.h"
#include "MapToolScene.h"

MapToolScene::MapToolScene()
{
	Environment::Get()->SetIsEnabledTargetCamera(false);
	WORLDCAMERA->mPosition = { -35.2f,113.6f, -105.7f };
	WORLDCAMERA->mRotation = { 0.5f,0.4f, 0.0f };
	WORLDCAMERA->mMoveSpeed = 200.0f;
	WORLDCAMERA->mWheelSpeed = 30.0f;

	terrainEditor = new TerrainEditor();
	//skyBox = new SkyBox();

	mRasterizerState = new RasterizerState();
	mRasterizerState->FillMode(D3D11_FILL_WIREFRAME);
}

MapToolScene::~MapToolScene()
{
	delete terrainEditor;
	//delete skyBox;
	delete mRasterizerState;
}

void MapToolScene::Update()
{
	if (Environment::Get()->GetIsEnabledTargetCamera())
	{
		Environment::Get()->GetTargetCamera()->Update();
	}

	Environment::Get()->GetWorldCamera()->Update();

	terrainEditor->Update();
}

void MapToolScene::PreRender()
{
	Environment::Get()->SetPerspectiveProjectionBuffer();
	Environment::Get()->Set(); // ºä¹öÆÛ Set VS

	if (Environment::Get()->GetIsEnabledTargetCamera())
	{
		Environment::Get()->GetTargetCamera()->Render();
	}

	Environment::Get()->GetWorldCamera()->Render();

	terrainEditor->PreRender();
}

void MapToolScene::Render()
{
	// ¹é¹öÆÛ¿Í ¿¬°áµÈ ·»´õÅ¸°Ù
	Device::Get()->ClearRenderTargetView(Float4(0.18f, 0.18f, 0.25f, 1.0f));
	Device::Get()->ClearDepthStencilView();
	Device::Get()->SetRenderTarget();

	if (Environment::Get()->GetIsEnabledTargetCamera())
	{
		Environment::Get()->GetTargetCamera()->Render();
	}

	Environment::Get()->GetWorldCamera()->Render(); // FrustumRender ¿Ü¿£ ¹¹ ¾÷½Â¤±.
	Environment::Get()->Set(); // SetViewPort
	Environment::Get()->SetPerspectiveProjectionBuffer();

	//skyBox->Render();
	//mRasterizerState->SetState();
	terrainEditor->Render();
}

void MapToolScene::PostRender()
{
	Environment::Get()->SetPerspectiveProjectionBuffer();

	terrainEditor->PostRender();
	//ImGui::ShowDemoWindow();
	//ImGui::ShowMetricsWindow();
}
