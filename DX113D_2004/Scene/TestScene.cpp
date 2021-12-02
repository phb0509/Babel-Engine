#include "Framework.h"
#include "TestScene.h"



TestScene::TestScene()
{
	monster = new Mutant();
	terrain = new Terrain();

	monster->SetTerrain(terrain);

	monster->mPosition = { 40.0f,0.0f,40.0f };
}

TestScene::~TestScene()
{
}

void TestScene::Update()
{
	terrain->Update();
	monster->Update();
}

void TestScene::PreRender()
{
}

void TestScene::Render()
{
	terrain->Render();
	monster->Render();
}

void TestScene::PostRender()
{
	
}


