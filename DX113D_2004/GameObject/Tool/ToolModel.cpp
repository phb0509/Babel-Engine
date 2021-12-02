#include "Framework.h"

ToolModel::ToolModel(string name) : 
	ModelAnimator(),
	mClipIndex(0), 
	mCurrentClipIndex(0)
{
	mName = name;
	mScale = { 0.05f,0.05f,0.05f }; // 기본모델이 너무 커서 줄여야됨.
}

ToolModel::~ToolModel()
{
}

void ToolModel::Update()
{
	if (GetHasMeshes())
	{
		UpdateWorld();
		ModelAnimator::Update();
	}
}

void ToolModel::Render()
{
	if (GetHasMeshes())
	{
		SetWorldBuffer();
		ModelAnimator::Render();
	}
}

void ToolModel::PostRender()
{

}

void ToolModel::SetAnimation(int _clipIndex)
{
	if (mClipIndex != _clipIndex)
	{
		mClipIndex = _clipIndex;
		PlayClip(mClipIndex);
	}
}




