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
		Attack, // ���Ž������̵� ����, ���ݻ��¸� �ϴ�?
		OnDamage,
		Die,
	};
}