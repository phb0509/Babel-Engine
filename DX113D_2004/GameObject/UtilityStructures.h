#pragma once

struct ColliderData
{
	string colliderName;
	string nodeName;
	UINT colliderType;

	Vector3 position;
	Vector3 rotation;
	Vector3 scale;
};

struct TempCollider
{
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;
};

struct SettedCollider
{
	string colliderName;
	string nodeName;
	Matrix matrix;
	Collider* collider;
};

struct InstanceColliderData
{
	vector<SettedCollider> colliders;
	map<string, Collider*> collidersMap; // key : ColliderName value : Collider

	InstanceColliderData() {};
	InstanceColliderData(vector<SettedCollider> _colliders, map<string, Collider*> _collidersMap) : colliders(_colliders), collidersMap(_collidersMap) {};
};

struct AttackInformation
{
	string attackName;
	//Collider* attackCollider;
	vector<Collider*> attackColliders; 
	eAttackType attackType;
	float damage;

	AttackInformation() {};
	AttackInformation(string _attackName, vector<Collider*> _attackColliders, eAttackType _attackType, float _damage) : 
		attackName(_attackName), attackColliders(_attackColliders), attackType(_attackType), damage(_damage) {};
};