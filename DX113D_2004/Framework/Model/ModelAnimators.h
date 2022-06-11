#pragma once

class Camera;

class ModelAnimators : public ModelAnimator
{
private:
	struct InstanceData
	{
		Matrix worldMatrix;
		UINT instanceIndex;
	};


public:
	ModelAnimators();
	~ModelAnimators();

	void Update() override;
	void UpdateTransforms();
	void Render() override;
	void DeferredRender();
	void PostRender();

	Transform* Add();
	void AddTransform(Transform* transform);
	void PlayClip(UINT instance, UINT clip, float speed = 1.0f, float takeTime = 0.2f);

	Transform* GetTransform(UINT instance) { return mTransforms[instance]; }

	void SetEndEvents(UINT instance, UINT clip, CallBackParam Event) { mEndEvents[instance][clip] = Event; }
	void SetParams(UINT instance, UINT clip, int param) { mParams[instance][clip] = param; }
	void SetDrawCount(int drawCount) { mDrawCount = drawCount; }
	void SetBoxForFrustumCulling() { SetBox(&mMinBox, &mMaxBox); }
	void SetCameraForCulling(Camera* cameraForFrustumCulling) { mCameraForFrustumCulling = cameraForFrustumCulling; }
	void SetIsFrustumCullingMode(bool value) { mbIsFrustumCullingMode = value; }
	void SetInstanceCount(int instanceCount);

protected:
	vector<int> mRenderedInstanceIndices;

private:
	vector<Transform*> mTransforms;
	//InstanceData mInstanceData[MAX_INSTANCE];
	vector<InstanceData> mInstanceData;
	VertexBuffer* mInstanceBuffer;

	Camera* mCameraForFrustumCulling;
	UINT mDrawCount;

	Vector3 mMinBox;
	Vector3 mMaxBox;

	vector<map<UINT, CallBackParam>> mEndEvents;
	vector<map<UINT, int>> mParams;
	bool mbIsFrustumCullingMode;
};