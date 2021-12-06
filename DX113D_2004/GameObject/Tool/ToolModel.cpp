#include "Framework.h"

ToolModel::ToolModel(string name) : 
	ModelAnimator(),
	mClipIndex(0), 
	mCurrentClipIndex(0),
	mName("")
{
	mName = name;
	mScale = { 0.05f,0.05f,0.05f }; // �⺻���� �ʹ� Ŀ�� �ٿ��ߵ�.
}

ToolModel::~ToolModel()
{
}

void ToolModel::Update()
{
	UpdateWorld();

	if (GetHasMeshes())
	{
		ModelAnimator::Update();
	}
	
}

void ToolModel::Render()
{
	SetWorldBuffer();

	if (GetHasMeshes())
	{
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

void ToolModel::SetClip(string modelName, string clipFileName)
{
	ReadClip(modelName, clipFileName); // Ȯ�������� ���ϸ�
	CreateTexture();
	PlayClip(mClips.size() - 1);
}




