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
	UpdateWorld();
	SetWorldBuffer();
	ModelAnimator::Render();
}

void ModelAnimObject::DeferredRender()
{
	UpdateWorld();
	SetWorldBuffer();
	ModelAnimator::DeferredRender();
}
