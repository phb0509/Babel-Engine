#pragma once

class ModelClip
{
private:
	friend class ModelAnimator;
	friend class PlayerModelAnimator;
	friend class ModelAnimators;

public:
	ModelClip();
	~ModelClip();

	KeyFrame* GetKeyFrame(string name);
	string GetName() { return mName; }
	UINT GetFrameCount() { return mFrameCount; }

private:
	string mName;

	float mDuration;
	float mFramePerSecond;
	UINT mFrameCount;

	unordered_map<string, KeyFrame*> mKeyFrames; // 노드이름, 해당 노드의 키프레임.
};