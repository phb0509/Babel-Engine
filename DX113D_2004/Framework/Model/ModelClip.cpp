#include "Framework.h"

ModelClip::ModelClip()
	: mDuration(0), mFramePerSecond(0), mFrameCount(0)
{
}

ModelClip::~ModelClip()
{
	for (auto frame : mKeyFrames)
		delete frame.second;
}

KeyFrame* ModelClip::GetKeyFrame(string name)
{
	if (mKeyFrames.count(name) == 0)
		return nullptr;

	return mKeyFrames[name]; // unordered_map<string, KeyFrame*> keyFrames;
}

//struct KeyFrame
//{
//	// �� �ϳ��� ������ ������ŭ�� Ű������ �����͸� ����.
//	string boneName;
//	vector<KeyTransform> transforms;
//};
