#pragma once

struct OBB
{
	Vector3 position;

	Vector3 axis[3];
	Vector3 halfSize;
};

class BoxCollider : public Collider
{
public:
	BoxCollider(
		Vector3 minBox = Vector3(-0.5f, -0.5f, -0.5f),
		Vector3 maxBox = Vector3(0.5f, 0.5f, 0.5f)
	);
	~BoxCollider();

	virtual bool RayCollision(IN Ray ray, OUT Contact* contact = nullptr) override;
	virtual bool BoxCollision(BoxCollider* collider) override;
	virtual bool SphereCollision(SphereCollider* collider) override;
	virtual bool CapsuleCollision(CapsuleCollider* collider) override;
	
	bool SphereCollision(Vector3 center, float radius);

	Vector3 MinBox();
	Vector3 MaxBox();

	OBB GetObb();

private:
	virtual void createMesh() override;
	virtual void createMeshForColorPicking() override;

	bool seperateAxis(Vector3 D, Vector3 axis, OBB box1, OBB box2);

private:
	Vector3 mMinBox;
	Vector3 mMaxBox;

	OBB mOBB;
};