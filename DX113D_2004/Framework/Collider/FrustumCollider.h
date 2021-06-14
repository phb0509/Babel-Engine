#pragma once

class FrustumCollider : public Collider
{
public:
	FrustumCollider(float nearRectWidth, float nearRectHeight, float farRectWidth, float farRectHeight, float distanceToNearRect, float distanceToFarRect);
	~FrustumCollider();

	// Collider��(��) ���� ��ӵ�
	virtual bool RayCollision(IN Ray ray, OUT Contact* contact = nullptr) override;
	virtual bool BoxCollision(BoxCollider* collider) override;
	virtual bool SphereCollision(SphereCollider* collider) override;
	virtual bool CapsuleCollision(CapsuleCollider* collider) override;

private:
	virtual void CreateMesh() override;


private:
	float mNearRectWidth;
	float mNearRectHeight;
	float mFarRectWidth;
	float mFarRectHeight;
	float mDistanceToNearRect;
	float mDistanceToFarRect;
};