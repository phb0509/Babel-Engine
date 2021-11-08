#include "Framework.h"
#include "TestScene.h"
#include <iostream>

void threadFunc()
{
	ModelExporter* exporter = new ModelExporter("ModelData/Mutant.fbx");
	exporter->ExportMaterial("Mutant/Mutant");
	exporter->ExportMesh("Mutant/Mutant");
}

TestScene::TestScene()
{
	thread t1(threadFunc);

	t1.join();

}

TestScene::~TestScene()
{
}

void TestScene::Update()
{

}

void TestScene::PreRender()
{
}

void TestScene::Render()
{

}

void TestScene::PostRender()
{
	
}


