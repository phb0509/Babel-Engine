#include "Framework.h"

SphereCollider::SphereCollider(float radius, UINT stackCount, UINT sliceCount)
    : mRadius(radius), mStackCount(stackCount), mSliceCount(sliceCount)
{
    mType = eType::SPHERE;
    createMesh();
	createMeshForColorPicking();
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



void SphereCollider::createMesh()
{
	float phiStep = XM_PI / mStackCount;
	float thetaStep = XM_2PI / mSliceCount;

	for (UINT i = 0; i <= mStackCount; i++)
	{
		float phi = i * phiStep;

		for (UINT j = 0; j <= mSliceCount; j++)
		{
			float theta = j * thetaStep;

			Vertex vertex;

			vertex.position.x = sin(phi) * cos(theta) * mRadius;
			vertex.position.y = cos(phi) * mRadius;
			vertex.position.z = sin(phi) * sin(theta) * mRadius;

			mVertices.emplace_back(vertex);
		}
	}

	for (UINT i = 0; i < mStackCount; i++)
	{
		for (UINT j = 0; j < mSliceCount; j++)
		{
			mIndices.emplace_back((mSliceCount + 1) * i + j);//0
			mIndices.emplace_back((mSliceCount + 1) * i + j + 1);//1			

			mIndices.emplace_back((mSliceCount + 1) * i + j);//0
			mIndices.emplace_back((mSliceCount + 1) * (i + 1) + j);//2
		}
	}

	mMesh = new Mesh(mVertices.data(), sizeof(Vertex), mVertices.size(),
		mIndices.data(), mIndices.size());
}

void SphereCollider::createMeshForColorPicking()
{
	float phiStep = XM_PI / mStackCount;
	float thetaStep = XM_2PI / mSliceCount;

	for (UINT i = 0; i <= mStackCount; i++)
	{
		float phi = i * phiStep;

		for (UINT j = 0; j <= mSliceCount; j++)
		{
			float theta = j * thetaStep;

			Vertex vertex;

			vertex.position.x = sin(phi) * cos(theta);
			vertex.position.y = cos(phi);
			vertex.position.z = sin(phi) * sin(theta);

			vertex.position = Vector3(vertex.position) * mRadius;

			//vertex.uv.x = (float)j / sliceCount;
			//vertex.uv.y = (float)i / stackCount;

			mVerticesForColorPicking.emplace_back(vertex);
		}
	}

	for (UINT i = 0; i < mStackCount; i++)
	{
		for (UINT j = 0; j < mSliceCount; j++)
		{
			mIndicesForColorPicking.emplace_back((mSliceCount + 1) * i + j);
			mIndicesForColorPicking.emplace_back((mSliceCount + 1) * i + j + 1);
			mIndicesForColorPicking.emplace_back((mSliceCount + 1) * (i + 1) + j);

			mIndicesForColorPicking.emplace_back((mSliceCount + 1) * (i + 1) + j);
			mIndicesForColorPicking.emplace_back((mSliceCount + 1) * i + j + 1);
			mIndicesForColorPicking.emplace_back((mSliceCount + 1) * (i + 1) + j + 1);
		}
	}

	mMeshForColorPicking = new Mesh(mVerticesForColorPicking.data(), sizeof(Vertex), mVerticesForColorPicking.size(),
		mIndicesForColorPicking.data(), mIndicesForColorPicking.size());
}