#pragma once

class TetrahedronCollider : public Collider
{
public:
	TetrahedronCollider(float rectWidth = 10.0f, float rectHeight = 10.0f, float distanceToRect = 5.0f);
	~TetrahedronCollider();

	virtual bool RayCollision(IN Ray ray, OUT Contact* contact = nullptr) override;
	virtual bool BoxCollision(BoxCollider* collider) override;
	virtual bool SphereCollision(SphereCollider* collider) override;
	virtual bool CapsuleCollision(CapsuleCollider* collider) override;
	
private:
	virtual void createMesh() override;
	virtual void createMeshForColorPicking() override;

private:
	float mRectWidth;
	float mRectHeight;
	float mDistanceToRect;
};