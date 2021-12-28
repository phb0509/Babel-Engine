#pragma once

class CapsuleCollider : public Collider
{

public:
	CapsuleCollider(float radius = 1.0f, float height = 2.0f,
		UINT stackCount = 15, UINT sliceCount = 30);
	~CapsuleCollider();

	virtual bool RayCollision(IN Ray ray, OUT Contact* contact = nullptr) override;
	virtual bool BoxCollision(BoxCollider* collider) override;
	virtual bool SphereCollision(SphereCollider* collider) override;
	virtual bool CapsuleCollision(CapsuleCollider* collider) override;	
	

	float Radius() { return mRadius * max(GetGlobalScale().x, max(GetGlobalScale().y, GetGlobalScale().z)); }
	float Height() { return mHeight * GetGlobalScale().y; }

private:
	virtual void createMesh() override;
	virtual void createMeshForColorPicking() override;

private:
	float mRadius;
	float mHeight;

	UINT mStackCount;
	UINT mSliceCount;
};