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
//	// 본 하나당 프레임 개수만큼의 키프레임 데이터를 가짐.
//	string boneName;
//	vector<KeyTransform> transforms;
//};
