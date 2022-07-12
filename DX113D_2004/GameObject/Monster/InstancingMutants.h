#pragma once


class InstancingMutants : public ModelAnimators
{
public:

	friend class Player;

	InstancingMutants(int instanceCount, Terrain* terrain);
	~InstancingMutants();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void RenderDebugMode();

	void OnDamage(int instanceIndex, AttackInformation attackInformation);
	void CheckOnHit(int instanceIndex);

	vector<Monster*> GetInstanceObjects() { return mInstanceObjects; }
	Collider* GetColliderForAStar(int instanceIndex);
	Collider* GetHitCollider(int instanceIndex);
	eMutantAnimationStates GetAnimationStates() { return mAnimationState; }

	void SetAnimationStates(eMutantAnimationStates animationStates) { mAnimationState = animationStates; }
	void SetIdle(int instanceIndex);
	void SetAnimation(int instanceIndex, eMutantAnimationStates value);
	void SetCamera(Camera* camera) { mCamera = camera; }

private:
	void setOnDamageEnd(int instanceIndex);
	void setColliders();
	void setAttackEnd(int instanceIndex);
	void renderColliders();
	void loadBinaryCollidersFile(wstring fileName);
	void showInstanceInformation();
	void updateCompletedAnimIndices(bool value);

private:
	Camera* mCamera;
	int mInstanceCount;
	vector<Monster*> mInstanceObjects;
	eMutantAnimationStates mAnimationState;
	eMutantFSMStates mFSM;
	Terrain* mTerrain;

	vector<TempCollider> mTempColliderSRTdatas;
	vector<ColliderData> mTempColliderDatas;
	vector<InstanceColliderData> mInstanceColliderDatas;
};