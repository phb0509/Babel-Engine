#include "Framework.h"


DebugManager::DebugManager():
	mbIsDebugMode(false)
{
}

DebugManager::~DebugManager()
{
}

void DebugManager::Update()
{
	if (KEY_DOWN('0'))
	{
		mbIsDebugMode = !mbIsDebugMode;
	}
}


void DebugManager::AddCollider(Collider* collider)
{
	mColliders.push_back(collider);
}

