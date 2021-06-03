#include "Framework.h"

ToolModel::ToolModel(string file) : ModelAnimator(file + "/" + file) ,clipIndex(0), currentClipIndex(0)
{
	mScale = { 0.05f,0.05f,0.05f };
	SetShader(L"ModelAnimation");
}

ToolModel::~ToolModel()
{
}

void ToolModel::Update()
{
	UpdateWorld();
	ModelAnimator::Update();
}

void ToolModel::Render()
{
	SetWorldBuffer();
	ModelAnimator::Render();
}

void ToolModel::PostRender()
{

}

void ToolModel::SetAnimation(int _clipIndex)
{
	if (clipIndex != _clipIndex)
	{
		clipIndex = _clipIndex;
		PlayClip(clipIndex);
	}
}


