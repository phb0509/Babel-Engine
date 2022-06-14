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
	map<string, Collider*> collidersMap; // key : NodeName value : Collider

	InstanceColliderData() {};
	InstanceColliderData(vector<SettedCollider> _colliders, map<string, Collider*> _collidersMap) : colliders(_colliders), collidersMap(_collidersMap) {};
};
