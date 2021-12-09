#include "Framework.h"
#include "TestScene.h"



TestScene::TestScene()
{
	mMonster = new Mutant();
	terrain = new Terrain();

	mMonster->SetTerrain(terrain);

	mMonster->mPosition = { 40.0f,0.0f,40.0f };
}

TestScene::~TestScene()
{
}

void TestScene::Update()
{
	terrain->Update();
	mMonster->Update();
}

void TestScene::PreRender()
{
}

void TestScene::Render()
{
	terrain->Render();
	mMonster->Render();
}

void TestScene::PostRender()
{
	
}


