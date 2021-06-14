#pragma once

class TetrahedronCollider : public Collider
{
public:
	TetrahedronCollider(float rectWidth = 10.0f, float rectHeight = 10.0f, float distanceToRect = 5.0f);
	~TetrahedronCollider();

private:
	virtual void CreateMesh() override;

	// Collider을(를) 통해 상속됨
	virtual bool RayCollision(IN Ray ray, OUT Contact* contact = nullptr) override;
	virtual bool BoxCollision(BoxCollider* collider) override;
	virtual bool SphereCollision(SphereCollider* collider) override;
	virtual bool CapsuleCollision(CapsuleCollider* collider) override;



private:
	float mRectWidth;
	float mRectHeight;
	float mDistanceToRect;
};