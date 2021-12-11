#pragma once

struct Ray
{
	Vector3 position;
	Vector3 direction;
};

struct Contact
{
	Vector3 hitPoint;
	float distance;
};

class BoxCollider;
class SphereCollider;
class CapsuleCollider;

class Collider : public Transform
{



public:
	Collider();
	virtual ~Collider();

	bool Collision(Collider* collider);
	virtual bool RayCollision(IN Ray ray, OUT Contact* contact = nullptr) = 0;
	virtual bool BoxCollision(BoxCollider* collider) = 0;
	virtual bool SphereCollision(SphereCollider* collider) = 0;
	virtual bool CapsuleCollision(CapsuleCollider* collider) = 0;

	virtual void CreateMesh() = 0;

	void Update();
	void Render();

	void SetColor(Float4 color) { mMaterial->GetBuffer()->data.diffuse = color; }
	eType GetType() { return mType; }

protected:
	Material* mMaterial;
	Mesh* mMesh;

	eType mType;

	vector<Vertex> mVertices;
	vector<UINT> mIndices;

	bool mbIsRender;
};