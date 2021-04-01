#include "Framework.h"

ModelAnimObject::ModelAnimObject(string file)
	: ModelAnimator(file)
{
}

ModelAnimObject::~ModelAnimObject()
{
}

void ModelAnimObject::Update()
{
	ModelAnimator::Update();
}

void ModelAnimObject::Render()
{
	SetWorldBuffer();
	ModelAnimator::Render();
	RenderAxis();
}
