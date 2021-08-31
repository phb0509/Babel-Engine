#include "Framework.h"
#include "MapToolScene.h"

MapToolScene::MapToolScene()
{
	Environment::Get()->SetIsEnabledTargetCamera(false);
	WORLDCAMERA->mPosition = { -35.2f,113.6f, -105.7f };
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
	terrainEditor->Update();
	//cube->Update();
	//cube->mPosition = Environment::Get()->GetLightPosition();

}

void MapToolScene::PreRender()
{
	terrainEditor->PreRender();
}

void MapToolScene::Render()
{
	//skyBox->Render();
	//mRasterizerState->SetState();
	terrainEditor->Render();


}

void MapToolScene::PostRender()
{
	terrainEditor->PostRender();
	//ImGui::ShowDemoWindow();
	//ImGui::ShowMetricsWindow();
}
