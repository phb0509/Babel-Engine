#pragma once


class InstanceMutant : public Monster
{
public:
	InstanceMutant();
	~InstanceMutant();

	// Monster을(를) 통해 상속됨
	virtual void Update() override;
	virtual void InstanceUpdate() override;
	virtual void PreRender() override;
	virtual void Render() override;

	virtual Collider* GetHitCollider() override;
	virtual void OnDamage(float damage) override;
	virtual bool CheckOnDamage(const Collider* collider) override;
	virtual void CheckOnHit() override;
	virtual Collider* GetColliderForAStar() override;
	virtual void SetAnimation(eAnimationStates value) override;


private:


private:
	bool mbOnHit;
};