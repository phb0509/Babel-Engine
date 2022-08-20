#pragma once


class MutantPatrolState;
class MutantStalkingState;
class MutantAttackState;
class MutantOnDamageState;
class MutantDieState;
class MutantAttackedNormalState;
class MutantAttackedKnockBackState;

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
	virtual void OnDamage(AttackInformation attackInformation) override;
	virtual bool CheckIsCollision(Collider* collider) override;
	virtual void CheckOnHit() override;
	virtual Collider* GetColliderForAStar() override;
	virtual MonsterState* GetFSMState(int num) override;
	virtual int GetEnumFSMState() override;
	virtual void SetFSMState(int state) override;
	virtual int GetAnimationState() override;
	virtual void SetAnimation(int animationState, float speed = 1.0f, float takeTime = 0.2f, bool isForcingPlay = false) override;
	virtual void ReActivation() override;
	virtual void SetAttackInformations() override;

private:
	void setStalkingState();

private:
	eMutantAnimationStates mCurrentAnimationState;

	MutantPatrolState* mPatrolState;
	MutantStalkingState* mStalkingState;
	MutantAttackState* mAttackState;
	MutantDieState* mDieState;
	MutantAttackedNormalState* mAttackedNormalState;
	MutantAttackedKnockBackState* mAttackedKnockBackState;

	bool mbOnHit;
	float mSmashAttackDamage;
};