#pragma once


namespace states
{
	enum class eAnimationStates
	{
		Idle,
		Run,
		SmashAttack,
		OnDamage,
		Die,
	};

	enum class eStates
	{
		Patrol,
		Stalk,
		Attack, // 스매쉬어택이든 뭐든, 공격상태면 일단?
		OnDamage,
		Die,
	};
}