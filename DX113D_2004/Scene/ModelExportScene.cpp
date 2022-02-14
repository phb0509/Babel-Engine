#include "Framework.h"
#include "ModelExportScene.h"


ModelExportScene::ModelExportScene()
{
	//string name = "Mutant";
	//
	////exporter = new ModelExporter("ModelData/Models/" + name + ".fbx");
	//exporter = new ModelExporter("ModelData/" + name + ".fbx");
	//exporter->ExportMaterial(name + "/" + name);
	//exporter->ExportMesh(name + "/" + name);

	
	
	//string clip = "/OnDamage";
	//exporter = new ModelExporter("ModelData/Animations/" + name + clip + ".fbx");
	//exporter->ExportClip(name + clip);
	
	//delete exporter;
	//
	//clip = "/Run";
	//exporter = new ModelExporter("ModelData/Animations/" + name + clip + ".fbx");
	//exporter->ExportClip(name + clip);
	//delete exporter;

	//clip = "/SmashAttack";
	//exporter = new ModelExporter("ModelData/Animations/" + name + clip + ".fbx");
	//exporter->ExportClip(name + clip);

	//clip = "/Die";
	//exporter = new ModelExporter("ModelData/Animations/" + name + clip + ".fbx");
	//exporter->ExportClip(name + clip);
	
}

ModelExportScene::~ModelExportScene()
{
	delete exporter;
}

void ModelExportScene::Update()
{
	if (Environment::Get()->GetIsEnabledTargetCamera())
	{
		Environment::Get()->GetTargetCamera()->Update();
	}

	Environment::Get()->GetWorldCamera()->Update();
}

void ModelExportScene::PreRender()
{
	Environment::Get()->Set();
	Environment::Get()->SetPerspectiveProjectionBuffer();
}

void ModelExportScene::Render()
{
	Device::Get()->SetRenderTarget(); // SetMainRenderTarget

	if (Environment::Get()->GetIsEnabledTargetCamera())
	{
		Environment::Get()->GetTargetCamera()->Render();
	}

	Environment::Get()->GetWorldCamera()->Render();
	Environment::Get()->Set();
	Environment::Get()->SetPerspectiveProjectionBuffer();
}

void ModelExportScene::PostRender()
{
	Environment::Get()->SetPerspectiveProjectionBuffer();
}
