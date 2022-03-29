#include "Framework.h"

ModelAnimObject::ModelAnimObject()
	: ModelAnimator()
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
}
