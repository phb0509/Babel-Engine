#include "Framework.h"
#include "TestScene.h"

TestScene::TestScene()
{


	
}

TestScene::~TestScene()
{
}

void TestScene::Update()
{
	for (int i = 0; i < cubes.size(); i++)
	{
		cubes[i]->Update();
	}
}

void TestScene::PreRender()
{
}

void TestScene::Render()
{
	for (int i = 0; i < cubes.size(); i++)
	{
		cubes[i]->Render();
	}
}

void TestScene::PostRender()
{
}
