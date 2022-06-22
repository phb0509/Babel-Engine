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

	enum class eMutantFSMstates
	{
		Patrol,
		Stalk,
		Attack, // ���Ž������̵� ����, ���ݻ��¸� �ϴ�?
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