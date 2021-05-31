#pragma once

class TetrahedronCollider : public Collider
{
public:
	TetrahedronCollider(float rectSize = 5.0f, float distanceToRect = 5.0f);
	~TetrahedronCollider();

private:
	virtual void CreateMesh() override;

	// Collider��(��) ���� ��ӵ�
	virtual bool RayCollision(IN Ray ray, OUT Contact* contact = nullptr) override;
	virtual bool BoxCollision(BoxCollider* collider) override;
	virtual bool SphereCollision(SphereCollider* collider) override;
	virtual bool CapsuleCollision(CapsuleCollider* collider) override;



private:
	float mRectSize;
	float mDistanceToRect;
};