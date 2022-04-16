#pragma once


class InstanceMonster : public Monster
{
public:
	InstanceMonster();
	~InstanceMonster();

	// Monster을(를) 통해 상속됨
	virtual void Update() override;
	virtual void InstanceUpdate() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual Collider* GetHitCollider() override;
	virtual void OnDamage(float damage) override;
	virtual void CheckOnHit() override;
	virtual Collider* GetColliderForAStar() override;
	virtual void SetAnimation(eAnimationStates value) override;


private:


private:
	//vector<TempCollider> mColliderSRTdatas;
	//vector<ColliderData> mColliderDatas;
	//vector<SettedCollider> mColliders;
	//map<string, Collider*> mCollidersMap;

	bool mbOnHit;
};