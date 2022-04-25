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
	UpdateWorld();
	ModelAnimator::Update();
}

void ModelAnimObject::Render()
{
	SetWorldBuffer();
	ModelAnimator::Render();
}

void ModelAnimObject::DeferredRender()
{
	SetWorldBuffer();
	ModelAnimator::DeferredRender();
}
