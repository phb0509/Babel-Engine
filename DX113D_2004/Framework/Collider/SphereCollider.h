#pragma once

class SphereCollider : public Collider
{


public:
	SphereCollider(float radius = 1.0f, UINT stackCount = 15, UINT sliceCount = 30);
	~SphereCollider();

	virtual bool RayCollision(IN Ray ray, OUT Contact* contact = nullptr) override;
	virtual bool BoxCollision(BoxCollider* collider) override;
	virtual bool SphereCollision(SphereCollider* collider) override;
	virtual bool CapsuleCollision(CapsuleCollider* collider) override;
	
	float Radius() { return mRadius * max(GetGlobalScale().x, max(GetGlobalScale().y, GetGlobalScale().z)); }

private:
	virtual void createMesh() override;
	virtual void createMeshForColorPicking() override;

private:
	float mRadius;

	UINT mStackCount;
	UINT mSliceCount;
};