#pragma once


class InstancingMutants : public ModelAnimators
{
public:

	friend class Player;

	struct ColliderData
	{
		string colliderName;
		string nodeName;
		int colliderType;

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

	InstancingMutants(int instanceCount, Terrain* terrain);
	~InstancingMutants();

	void Update();
	void PreRender();
	void Render();
	void PostRender();

	Collider* GetHitCollider(int instanceIndex);
	void OnDamage(int instanceIndex, float damage);
	void CheckOnHit(int instanceIndex);
	Collider* GetColliderForAStar(int instanceIndex);

	void SetIdle(int instanceIndex);
	void SetAnimation(int instanceIndex, eAnimationStates value);

	vector<Monster*> GetInstanceObjects() { return mInstanceObjects; }

private:

	void setOnDamageEnd(int instanceIndex);
	void setColliders(int instanceIndex);
	void setAttackEnd(int instanceIndex);
	void loadBinaryFile();

private:

	vector<Monster*> mInstanceObjects;
	Terrain* mTerrain;
};