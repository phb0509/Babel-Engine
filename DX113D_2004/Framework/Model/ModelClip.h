#pragma once

class ModelClip
{
private:
	friend class ModelAnimator;
	friend class ModelAnimators;

	string name;

	float duration;
	float tickPerSecond;
	UINT frameCount;

	unordered_map<string, KeyFrame*> keyFrames;

public:
	ModelClip();
	~ModelClip();

	KeyFrame* GetKeyFrame(string name);
};