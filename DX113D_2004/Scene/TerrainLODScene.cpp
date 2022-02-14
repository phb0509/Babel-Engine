#include "Framework.h"
#include "TerrainLODScene.h"

TerrainLODScene::TerrainLODScene()
{
	terrain = new TerrainLOD(L"Textures/HeightMaps/HeightMap.png");
	rsState = new RasterizerState();
	rsState->FillMode(D3D11_FILL_WIREFRAME);
}

TerrainLODScene::~TerrainLODScene()
{
	delete terrain;
	delete rsState;
}

void TerrainLODScene::Update()
{
	if (Environment::Get()->GetIsEnabledTargetCamera())
	{
		Environment::Get()->GetTargetCamera()->Update();
	}

	Environment::Get()->GetWorldCamera()->Update();

	terrain->Update();
}

void TerrainLODScene::PreRender()
{
	Environment::Get()->Set();
	Environment::Get()->SetPerspectiveProjectionBuffer();
}

void TerrainLODScene::Render()
{
	Device::Get()->SetRenderTarget(); // SetMainRenderTarget

	if (Environment::Get()->GetIsEnabledTargetCamera())
	{
		Environment::Get()->GetTargetCamera()->Render();
	}

	Environment::Get()->GetWorldCamera()->Render();
	Environment::Get()->Set();
	Environment::Get()->SetPerspectiveProjectionBuffer();

	rsState->SetState();
	terrain->Render();
}

void TerrainLODScene::PostRender()
{
	Environment::Get()->SetPerspectiveProjectionBuffer();

	terrain->PostRender();
}
