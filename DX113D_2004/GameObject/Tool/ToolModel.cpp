#include "Framework.h"

ToolModel::ToolModel(string name) : 
	ModelAnimator(),
	mClipIndex(0), 
	mCurrentClipIndex(0)
{
	mName = name;
	mScale = { 0.05f,0.05f,0.05f }; // �⺻���� �ʹ� Ŀ�� �ٿ��ߵ�.
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




