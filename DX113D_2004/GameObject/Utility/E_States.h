#pragma once


namespace states
{
	enum class eMutantAnimationStates
	{
		Idle,
		Run,
		SmashAttack,
		OnDamage,
		Die,
	};

	enum class eMutantFSMStates
	{
		Patrol,
		Stalk,
		Attack, 
		OnDamage,
		Die,
	};

	enum class eColliderType
	{
		BOX,
		SPHERE,
		CAPSULE,
		NONE
	};

	enum class eAttackType
	{
		Normal,
		KnockBack
	};
}