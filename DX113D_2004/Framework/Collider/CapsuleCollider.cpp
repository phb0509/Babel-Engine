#include "Framework.h"

CapsuleCollider::CapsuleCollider(float radius, float height, UINT stackCount, UINT sliceCount)
    : mRadius(radius), mHeight(height), mStackCount(stackCount), mSliceCount(sliceCount)
{
    mType = eColliderType::CAPSULE;
    createMesh();
	createMeshForColorPicking();
}

CapsuleCollider::~CapsuleCollider()
{
}

bool CapsuleCollider::RayCollision(IN Ray ray, OUT Contact* contact)
{
	Vector3 direction = Up();

	Vector3 pa = GetGlobalPosition() - direction * Height() * 0.5f;
	Vector3 pb = GetGlobalPosition() + direction * Height() * 0.5f;

	Vector3 ro = ray.position;
	Vector3 rd = ray.direction;

	float r = Radius();

	Vector3 ba = pb - pa;
	Vector3 oa = ro - pa;

	float baba = Vector3::Dot(ba, ba);
	float bard = Vector3::Dot(ba, rd);
	float baoa = Vector3::Dot(ba, oa);
	float rdoa = Vector3::Dot(rd, oa);
	float oaoa = Vector3::Dot(oa, oa);

	float a = baba - bard * bard;
	float b = baba * rdoa - baoa * bard;
	float c = baba * oaoa - baoa * baoa - r * r * baba;
	float h = b * b - a * c;

	if (h >= 0.0f)
	{
		float t = (-b - sqrt(h)) / a;

		float y = baoa + t * bard;

		if (y > 0.0f && y < baba)//Body
		{
			if (contact != nullptr)
			{
				contact->distance = t;
				contact->hitPoint = ray.position + ray.direction * t;
			}			
			return true;
		}

		Vector3 oc = (y <= 0.0f) ? oa : ro - pb;
		b = Vector3::Dot(rd, oc);
		c = Vector3::Dot(oc, oc) - r * r;
		h = b * b - c;
		if (h > 0.0f)
		{
			if (contact != nullptr)
			{
				contact->distance = -b - sqrt(h);
				contact->hitPoint = ray.position + ray.direction * contact->distance;
			}
			return true;
		}
	}

    return false;
}

bool CapsuleCollider::BoxCollision(BoxCollider* collider)
{
	Vector3 direction = Up();
	Vector3 startPos = GetGlobalPosition() - direction * Height() * 0.5f;

	Vector3 A = collider->GetGlobalPosition() - startPos;

	float t = Vector3::Dot(A, direction);
	t = max(0, t);
	t = min(Height(), t);

	Vector3 pointOnLine = startPos + direction * t;	

	return collider->SphereCollision(pointOnLine, Radius());
}

bool CapsuleCollider::SphereCollision(SphereCollider* collider)
{
	Vector3 direction = Up();
	Vector3 startPos = GetGlobalPosition() - direction * Height() * 0.5f;

	Vector3 A = collider->GetGlobalPosition() - startPos;

	float t = Vector3::Dot(A, direction);
	t = max(0, t);
	t = min(Height(), t);

	Vector3 pointOnLine = startPos + direction * t;

	float distance = Distance(pointOnLine, collider->GetGlobalPosition());

    return distance <= (Radius() + collider->Radius());
}

bool CapsuleCollider::CapsuleCollision(CapsuleCollider* collider)
{
	Vector3 aDirection = Up();

	Vector3 aA = GetGlobalPosition() - aDirection * Height() * 0.5f;
	Vector3 aB = GetGlobalPosition() + aDirection * Height() * 0.5f;

	Vector3 bDirection = collider->Up();

	Vector3 bA = collider->GetGlobalPosition() - bDirection * collider->Height() * 0.5f;
	Vector3 bB = collider->GetGlobalPosition() + bDirection * collider->Height() * 0.5f;

	Vector3 v0 = bA - aA;
	Vector3 v1 = bB - aA;
	Vector3 v2 = bA - aB;
	Vector3 v3 = bB - aB;

	float d0 = Vector3::Dot(v0, v0);
	float d1 = Vector3::Dot(v1, v1);
	float d2 = Vector3::Dot(v2, v2);
	float d3 = Vector3::Dot(v3, v3);

	Vector3 bestA;
	if (d2 < d0 || d2 < d1 || d3 < d0 || d3 > d1)
		bestA = aB;
	else
		bestA = aA;

	Vector3 bestB = ClosestPointOnLineSegment(bA, bB, bestA);
	bestA = ClosestPointOnLineSegment(aA, aB, bestB);

	float distance = Distance(bestA, bestB);

    return distance <= (Radius() + collider->Radius());
}



void CapsuleCollider::createMesh()
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

			if (vertex.position.y > 0)
				vertex.position.y += mHeight * 0.5f;
			else
				vertex.position.y -= mHeight * 0.5f;

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

void CapsuleCollider::createMeshForColorPicking()
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

			if (vertex.position.y > 0)
				vertex.position.y += mHeight * 0.5f;
			else
				vertex.position.y -= mHeight * 0.5f;

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