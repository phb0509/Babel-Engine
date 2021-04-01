#include "Framework.h"

ModelClip::ModelClip()
	: duration(0), tickPerSecond(0), frameCount(0)
{
}

ModelClip::~ModelClip()
{
	for (auto frame : keyFrames)
		delete frame.second;
}

KeyFrame* ModelClip::GetKeyFrame(string name)
{
	if (keyFrames.count(name) == 0)
		return nullptr;

	return keyFrames[name];
}
