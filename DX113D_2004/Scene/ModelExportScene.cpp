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
}

void ModelExportScene::PreRender()
{
}

void ModelExportScene::Render()
{
}

void ModelExportScene::PostRender()
{
}
