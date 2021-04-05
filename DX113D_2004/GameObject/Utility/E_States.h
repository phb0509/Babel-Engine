#pragma once


namespace states
{
	enum class eAnimation
	{
		Idle,
		Run,
		SmashAttack,
		OnDamage,
		Die,
	};

	enum class eFSM
	{
		Patrol,
		Stalk,
		Attack, // 스매쉬어택이든 뭐든, 공격상태면 일단?
		OnDamage,
		Die,
	};
}