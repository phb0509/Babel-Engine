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

	enum class eFSMstates
	{
		Patrol,
		Stalk,
		Attack, // ���Ž������̵� ����, ���ݻ��¸� �ϴ�?
		OnDamage,
		Die,
	};

	enum class eType
	{
		BOX,
		SPHERE,
		CAPSULE,
		NONE
	};
	
}