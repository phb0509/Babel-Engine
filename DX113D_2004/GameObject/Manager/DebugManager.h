#pragma once

class DebugManager : public Singleton<GameManager>
{
private:
	friend class Singleton;

	DebugManager();
	~DebugManager();

public:

	void AddCollider(Collider* collider);
;

private:

	vector<Collider*> mColliders;
};