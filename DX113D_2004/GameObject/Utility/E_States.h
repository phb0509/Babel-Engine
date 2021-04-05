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
		Attack, // ���Ž������̵� ����, ���ݻ��¸� �ϴ�?
		OnDamage,
		Die,
	};
}