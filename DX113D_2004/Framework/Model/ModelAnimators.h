#pragma once

class Frustum;

class ModelAnimators : public ModelAnimator
{
private:
	struct InstanceData
	{
		Matrix world;
		UINT index;
	};


public:
	ModelAnimators();
	~ModelAnimators();

	void Update() override;
	void Render() override;
	void PostRender();

	Transform* Add();

	void PlayClip(UINT instance, UINT clip,
		float speed = 1.0f, float takeTime = 0.2f);

	void UpdateTransforms();

	Transform* GetTransform(UINT instance) { return mTransforms[instance]; }

	void SetEndEvents(UINT instance, UINT clip, CallBackParam Event) { mEndEvents[instance][clip] = Event; }
	void SetParams(UINT instance, UINT clip, int param) { mParams[instance][clip] = param; }


private:
	vector<Transform*> mTransforms;
	InstanceData mInstanceData[MAX_INSTANCE];

	VertexBuffer* mInstanceBuffer;

	Frustum* mTargetCameraFrustum;
	UINT mDrawCount;

	Vector3 mMinBox, mMaxBox;

	vector<map<UINT, CallBackParam>> mEndEvents;
	vector<map<UINT, int>> mParams;
};