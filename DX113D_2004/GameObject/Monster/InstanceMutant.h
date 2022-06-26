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
	virtual int GetAnimationStates() override;
	virtual MonsterState* GetState(int num) override;
	virtual void SetAnimation(int value) override;

	/*static MutantPatrolState* GetPatrolState() { return mPatrolState; }
	static MutantStalkingState* GetStalkingState() { return mStalkingState; }
	static MutantAttackState* GetAttackState() { return mAttackState; }
	static MutantOnDamageState* GetOnDamageState() { return mOnDamageState; }
	static MutantDieState* GetDieState() { return mDieState; }*/

private:
	eMutantAnimationStates mCurrentAnimationState;
	eMutantFSMStates mCurrentFSMState;

	MutantPatrolState* mPatrolState;
	MutantStalkingState* mStalkingState;
	MutantAttackState* mAttackState;
	MutantOnDamageState* mOnDamageState;
	MutantDieState* mDieState;

	bool mbOnHit;
};