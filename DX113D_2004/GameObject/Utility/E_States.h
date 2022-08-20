#pragma once


namespace states
{
	// All
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

	// Mutant
	enum class eMutantAnimationStates
	{
		Idle,
		Run,
		SmashAttack,
		AttackedNormal,
		Die,
		AttackedKnockBack
	};

	enum class eMutantFSMStates
	{
		Patrol,
		Stalk,
		Attack,
		AttackedNormal,
		Die,
		AttackedKnockBack
	};
}