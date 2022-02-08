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
	terrain->Update();
}

void TerrainLODScene::PreRender()
{
	Environment::Get()->SetPerspectiveProjectionBuffer();
}

void TerrainLODScene::Render()
{
	Environment::Get()->SetPerspectiveProjectionBuffer();

	rsState->SetState();
	terrain->Render();
}

void TerrainLODScene::PostRender()
{
	Environment::Get()->SetPerspectiveProjectionBuffer();

	terrain->PostRender();
}
