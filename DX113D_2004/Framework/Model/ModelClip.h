#pragma once

class ModelClip
{
private:
	friend class ModelAnimator;
	friend class ModelAnimators;



public:
	ModelClip();
	~ModelClip();

	KeyFrame* GetKeyFrame(string name);

	string GetName() { return mName; }

private:
	string mName;

	float mDuration;
	float mFramePerSecond;
	UINT mFrameCount;

	unordered_map<string, KeyFrame*> mKeyFrames; // ����̸�, �ش� ����� Ű������.
};