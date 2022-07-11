#include "Framework.h"


DebugManager::DebugManager()
{
}

DebugManager::~DebugManager()
{
}

void DebugManager::AddCollider(Collider* collider)
{
	mColliders.push_back(collider);
}
