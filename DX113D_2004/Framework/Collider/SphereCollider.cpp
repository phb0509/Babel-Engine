#include "Framework.h"

SphereCollider::SphereCollider(float radius, UINT stackCount, UINT sliceCount)
    : radius(radius), stackCount(stackCount), sliceCount(sliceCount)
{
    mType = eType::SPHERE;
    CreateMesh();
}

SphereCollider::~SphereCollider()
{
}

bool SphereCollider::RayCollision(IN Ray ray, OUT Contact* contact)
{
	Vector3 P = ray.position;
	Vector3 D = ray.direction;

	Vector3 C = GetGlobalPosition();
	Vector3 A = P - C;

	float a = Vector3::Dot(D, D);
	float b = 2.0f * Vector3::Dot(D, A);
	float c = Vector3::Dot(A, A) - Radius() * Radius();

	if (b * b >= 4 * a * c)
	{
		if (contact != nullptr)
		{
			float t = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);

			contact->distance = t;
			contact->hitPoint = P + D * t;
		}

		return true;
	}

    return false;
}

bool SphereCollider::BoxCollision(BoxCollider* collider)
{
    return collider->SphereCollision(this);
}

bool SphereCollider::SphereCollision(SphereCollider* collider)
{
	float distance = Distance(GetGlobalPosition(), collider->GetGlobalPosition());

	float r1 = Radius();
	float r2 = collider->Radius();

    return distance <= r1 + r2;
}

bool SphereCollider::CapsuleCollision(CapsuleCollider* collider)
{
    return collider->SphereCollision(this);
}

void SphereCollider::CreateMesh()
{
	float phiStep = XM_PI / stackCount;
	float thetaStep = XM_2PI / sliceCount;

	for (UINT i = 0; i <= stackCount; i++)
	{
		float phi = i * phiStep;

		for (UINT j = 0; j <= sliceCount; j++)
		{
			float theta = j * thetaStep;

			Vertex vertex;

			vertex.position.x = sin(phi) * cos(theta) * radius;
			vertex.position.y = cos(phi) * radius;
			vertex.position.z = sin(phi) * sin(theta) * radius;

			mVertices.emplace_back(vertex);
		}
	}

	for (UINT i = 0; i < stackCount; i++)
	{
		for (UINT j = 0; j < sliceCount; j++)
		{
			mIndices.emplace_back((sliceCount + 1) * i + j);//0
			mIndices.emplace_back((sliceCount + 1) * i + j + 1);//1			

			mIndices.emplace_back((sliceCount + 1) * i + j);//0
			mIndices.emplace_back((sliceCount + 1) * (i + 1) + j);//2
		}
	}

	mMesh = new Mesh(mVertices.data(), sizeof(Vertex), mVertices.size(),
		mIndices.data(), mIndices.size());
}
