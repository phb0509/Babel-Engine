#pragma once

class Mutant : public Monster
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


	Mutant();
	~Mutant();

	virtual void Update() override;
	virtual void InstanceUpdate() override;
	virtual void PreRender() override;
	virtual void Render() override;
	void PostRender();

	virtual Collider* GetHitCollider() override;
	virtual void OnDamage(float damage) override;
	virtual void CheckOnHit() override;
	virtual Collider* GetColliderForAStar() override;

	void SetIdle();
	void SetAnimation(eAnimationStates value) override;


private:

	void setOnDamageEnd();
	void setColliders();
	void setAttackEnd();
	void loadBinaryFile();

private:

	vector<TempCollider> mColliderSRTdatas;
	vector<ColliderData> mColliderDatas;
	vector<SettedCollider> mColliders;
	map<string, Collider*> mCollidersMap;
	bool mbOnHit;
};