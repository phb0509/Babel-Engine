#include "Framework.h"

ModelObject::ModelObject(string file, Collider::Type type)
{
	model = new Model(file);
	model->SetShader(L"Lighting");	

	Vector3 minBox, maxBox;

	model->SetBox(&minBox, &maxBox);

	switch (type)
	{
	case Collider::BOX:
		collider = new BoxCollider(minBox, maxBox);
		break;
	case Collider::SPHERE:
		break;
	case Collider::CAPSULE:
		break;
	
	default:
		break;
	}
	if (collider != nullptr)
		collider->SetParent(&mWorldMatrix);
}

ModelObject::~ModelObject()
{
	delete model;
	if(collider != nullptr)
		delete collider;	
}

void ModelObject::Render()
{
	SetWorldBuffer();
	model->Render();
	if(collider != nullptr)
		collider->Render();
	RenderAxis();
}