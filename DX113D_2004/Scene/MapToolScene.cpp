#include "Framework.h"
#include "MapToolScene.h"

MapToolScene::MapToolScene()
{
	Environment::Get()->SetIsEnabledTargetCamera(false);
	//WORLDCAMERA->mPosition = { -35.2f,113.6f, -105.7f };
	terrainEditor = new TerrainEditor();
	//skyBox = new SkyBox();
	//cube = new Cube();

	mRasterizerState = new RasterizerState();
	mRasterizerState->FillMode(D3D11_FILL_WIREFRAME);
}

MapToolScene::~MapToolScene()
{
	delete terrainEditor;
	//delete skyBox;
	delete mRasterizerState;
	//delete cube;
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
	Environment::Get()->Set();
	Environment::Get()->SetPerspectiveProjectionBuffer();

	terrainEditor->PreRender();
}

void MapToolScene::Render()
{
	Device::Get()->SetRenderTarget(); // SetMainRenderTarget

	if (Environment::Get()->GetIsEnabledTargetCamera())
	{
		Environment::Get()->GetTargetCamera()->Render();
	}

	Environment::Get()->GetWorldCamera()->Render();
	Environment::Get()->Set();
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
