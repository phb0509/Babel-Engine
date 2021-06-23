#include "Framework.h"
#include "TestScene.h"

TestScene::TestScene()
{
	mTerrain = new Terrain();
	//mTerrainLOD = new TerrainLOD(L"Textures/HeightMaps/HeightMap.png");
//	rsState = new RasterizerState();
	//rsState->FillMode(D3D11_FILL_WIREFRAME);
	
}

TestScene::~TestScene()
{
}

void TestScene::Update()
{
	mTerrain->Update();
	//mTerrainLOD->Update();
	
}

void TestScene::PreRender()
{
}

void TestScene::Render()
{
	mTerrain->Render();
	//rsState->SetState();
//	mTerrainLOD->Render();
}

void TestScene::PostRender()
{
}
