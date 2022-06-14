#pragma once


class InstanceMutant : public Monster
{
public:
	InstanceMutant();
	~InstanceMutant();

	// Monster��(��) ���� ��ӵ�
	virtual void Update() override;
	virtual void InstanceUpdate() override;
	virtual void PreRender() override;
	virtual void Render() override;

	virtual Collider* GetHitCollider() override;
	virtual void OnDamage(float damage) override;
	virtual void CheckOnDamage(Collider* collider) override;
	virtual void CheckOnHit() override;
	virtual Collider* GetColliderForAStar() override;
	virtual void SetAnimation(eAnimationStates value) override;


private:


private:
	bool mbOnHit;
};