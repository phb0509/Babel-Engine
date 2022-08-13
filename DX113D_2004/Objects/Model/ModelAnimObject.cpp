#include "Framework.h"

ModelAnimObject::ModelAnimObject()
	: ModelAnimator(1,MAX_BONE,MAX_FRAME_KEY)
{
}

ModelAnimObject::~ModelAnimObject()
{
}

void ModelAnimObject::Update()
{
	Transform::UpdateWorld();
	ModelAnimator::Update();
}

void ModelAnimObject::Render()
{
	Transform::UpdateWorld();
	Transform::SetWorldBuffer();
	ModelAnimator::Render();
}

void ModelAnimObject::DeferredRender()
{
	Transform::UpdateWorld();
	Transform::SetWorldBuffer();
	ModelAnimator::DeferredRender();
}
