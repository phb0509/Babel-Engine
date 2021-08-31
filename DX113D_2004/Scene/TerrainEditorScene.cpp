#include "Framework.h"
#include "TerrainEditorScene.h"

TerrainEditorScene::TerrainEditorScene()
{
	terrainEditor = new TerrainEditor();
	//skyBox = new SkyBox();
}

TerrainEditorScene::~TerrainEditorScene()
{
	delete terrainEditor;
	//delete skyBox;
}

void TerrainEditorScene::Update()
{
	terrainEditor->Update();	
}

void TerrainEditorScene::PreRender()
{
}

void TerrainEditorScene::Render()
{
	//skyBox->Render();
	terrainEditor->Render();	
}

void TerrainEditorScene::PostRender()
{
	terrainEditor->PostRender();
	//skyBox->PostRender();
}
