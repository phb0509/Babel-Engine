#include "Framework.h"
#include "TestScene.h"

TestScene::TestScene()
{
	collider = new TetrahedronCollider(20.0f,30.0f);
}

TestScene::~TestScene()
{
}

void TestScene::Update()
{
	collider->Update();
}

void TestScene::PreRender()
{
}

void TestScene::Render()
{
	collider->Render();
}

void TestScene::PostRender()
{
}
