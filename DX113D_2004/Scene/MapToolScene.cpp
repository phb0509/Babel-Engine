#include "Framework.h"
#include "MapToolScene.h"

MapToolScene::MapToolScene()
{
	terrainEditor = new TerrainEditor();
	skyBox = new SkyBox();
	cube = new Cube();
}

MapToolScene::~MapToolScene()
{
	delete terrainEditor;
	delete skyBox;
	delete cube;
}

void MapToolScene::Update()
{
	terrainEditor->Update();
	cube->Update();
	cube->mPosition = Environment::Get()->GetLightPosition();

}

void MapToolScene::PreRender()
{
}

void MapToolScene::Render()
{
	skyBox->Render();
	terrainEditor->Render();
	cube->Render();

}

void MapToolScene::PostRender()
{
	terrainEditor->PostRender();
}
