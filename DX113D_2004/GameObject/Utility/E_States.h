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
		Attack, // 스매쉬어택이든 뭐든, 공격상태면 일단?
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
}