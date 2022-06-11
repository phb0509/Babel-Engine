#pragma once


class InstancingMutants : public ModelAnimators
{
public:

	friend class Player;

	struct ColliderData
	{
		string colliderName;
		string nodeName;
		UINT colliderType;

		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
	};

	struct TempCollider
	{
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
	};

	struct SettedCollider
	{
		string colliderName;
		string nodeName;
		Matrix matrix;
		Collider* collider;
	};

	struct InstanceColliderData
	{
		vector<SettedCollider> mColliders;
		map<string, Collider*> mCollidersMap;

		InstanceColliderData() {};
		InstanceColliderData(vector<SettedCollider> colliders, map<string, Collider*> collidersMap) : mColliders(colliders), mCollidersMap(collidersMap) {};
	};

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
	void loadBinaryCollidersFile();
	void showAnimationStates();

private:
	int mInstanceCount;
	vector<Monster*> mInstanceObjects;
	Terrain* mTerrain;

	vector<TempCollider> mTempColliderSRTdatas;
	vector<ColliderData> mTempColliderDatas;
	//vector<SettedCollider> mColliders;
	//map<string, Collider*> mCollidersMap;
	vector<InstanceColliderData> mInstanceColliderDatas;

};