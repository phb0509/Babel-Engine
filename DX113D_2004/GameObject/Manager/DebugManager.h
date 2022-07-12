#pragma once

class DebugManager : public Singleton<DebugManager>
{
private:
	friend class Singleton;

	DebugManager();
	~DebugManager();

public:
	void AddCollider(Collider* collider);
	bool GetIsDebugMode() { return mbIsDebugMode; }
	void Update();

private:
	vector<Collider*> mColliders;
	bool mbIsDebugMode;
};
