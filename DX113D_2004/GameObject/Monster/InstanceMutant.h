#pragma once


class MutantPatrolState;
class MutantStalkingState;
class MutantAttackState;
class MutantOnDamageState;
class MutantDieState;

class InstanceMutant : public Monster
{
public:
	InstanceMutant();
	~InstanceMutant();

	virtual void Update() override;
	virtual void InstanceUpdate() override;
	virtual void PreRender() override;
	virtual void Render() override;

	virtual Collider* GetHitCollider() override;
	virtual void OnDamage(float damage) override;
	virtual bool CheckOnDamage(const Collider* collider) override;
	virtual void CheckOnHit() override;
	virtual Collider* GetColliderForAStar() override;
	virtual MonsterState* GetFSMState(int num) override;
	virtual int GetEnumFSMState() override;
	virtual void SetFSMState(int state) override;
	virtual int GetAnimationState() override;
	virtual void SetAnimation(int value) override;

private:
	eMutantAnimationStates mCurrentAnimationState;

	MutantPatrolState* mPatrolState;
	MutantStalkingState* mStalkingState;
	MutantAttackState* mAttackState;
	MutantOnDamageState* mOnDamageState;
	MutantDieState* mDieState;

	bool mbOnHit;
};