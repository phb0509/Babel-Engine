#pragma once


class InstancingMutants : public ModelAnimators
{
public:

	friend class Player;

	InstancingMutants(int instanceCount, int nodeCount, int frameKeyCount, Terrain* terrain);
	~InstancingMutants();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void UIRender();
	void RenderDebugMode();

	vector<Monster*> GetInstanceObjects() { return mInstanceObjects; }
	eMutantAnimationStates GetAnimationStates() { return mAnimationState; }

	void SetAnimationStates(eMutantAnimationStates animationStates) { mAnimationState = animationStates; }
	void SetIdle(int instanceIndex);
	void SetAnimation(int instanceIndex, eMutantAnimationStates value);
	void SetCurMainCamera(Camera* mainCamera);

private:
	void setColliders();
	void renderColliders();
	void loadCollidersBinaryFile(wstring fileName);
	void showInstanceInformation();
	void updateCompletedAnimIndices(bool value);
	void updateStartedAnimIndices(bool value);

private:
	int mInstanceCount;
	vector<Monster*> mInstanceObjects;
	eMutantAnimationStates mAnimationState;
	eMutantFSMStates mFSM;
	Terrain* mTerrain;

	vector<TempCollider> mTempColliderSRTdatas;
	vector<ColliderData> mTempColliderDatas;
	vector<InstanceColliderData> mInstanceColliderDatas;

	Vector3 mUIOffset;
};