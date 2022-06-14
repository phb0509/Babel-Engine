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

	void OnDamage(int instanceIndex, float damage);
	void CheckOnHit(int instanceIndex);

	vector<Monster*> GetInstanceObjects() { return mInstanceObjects; }
	Collider* GetColliderForAStar(int instanceIndex);
	Collider* GetHitCollider(int instanceIndex);

	void SetIdle(int instanceIndex);
	void SetAnimation(int instanceIndex, eAnimationStates value);

private:
	void setOnDamageEnd(int instanceIndex);
	void setColliders();
	void setAttackEnd(int instanceIndex);
	void renderColliders();
	void loadBinaryCollidersFile(wstring fileName);
	void showAnimationStates();

private:
	int mInstanceCount;
	vector<Monster*> mInstanceObjects;
	Terrain* mTerrain;

	vector<TempCollider> mTempColliderSRTdatas;
	vector<ColliderData> mTempColliderDatas;
	vector<InstanceColliderData> mInstanceColliderDatas;
};