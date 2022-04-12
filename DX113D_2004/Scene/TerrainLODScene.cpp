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
	Environment::Get()->Set();

}

void TerrainLODScene::Render()
{
	Device::Get()->SetRenderTarget(); // SetMainRenderTarget


	Environment::Get()->Set();


	rsState->SetState();
	terrain->Render();
}

void TerrainLODScene::PostRender()
{
	terrain->PostRender();
}
