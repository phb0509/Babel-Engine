#include "Framework.h"

TetrahedronCollider::TetrahedronCollider(float rectWidth, float rectHeight, float distanceToRect) :
	mRectWidth(rectWidth),
	mRectHeight(rectHeight),
	mDistanceToRect(distanceToRect)
{
	CreateMesh();
}

TetrahedronCollider::~TetrahedronCollider()
{
}

void TetrahedronCollider::CreateMesh()
{
	vertices.emplace_back(0.0f, 0.0f, 0.0f);
	vertices.emplace_back(-mRectWidth / 2.0f, -mRectHeight / 2.0f, mDistanceToRect);
	vertices.emplace_back(-mRectWidth / 2.0f, mRectHeight / 2.0f, mDistanceToRect);
	vertices.emplace_back(mRectWidth / 2.0f, mRectHeight / 2.0f, mDistanceToRect);
	vertices.emplace_back(mRectWidth / 2.0f, -mRectHeight / 2.0f, mDistanceToRect);

	indices = {
		1,2,2,3,3,4,4,1, // 면 이어주기
		0,1,0,2,0,3,0,4  // 꼭지점과 면 이어주기.
	};

	mesh = new Mesh(vertices.data(), sizeof(Vertex), vertices.size(),
		indices.data(), indices.size());
}

bool TetrahedronCollider::RayCollision(IN Ray ray, OUT Contact* contact)
{
	return false;
}

bool TetrahedronCollider::BoxCollision(BoxCollider* collider)
{
	return false;
}

bool TetrahedronCollider::SphereCollision(SphereCollider* collider)
{
	return false;
}

bool TetrahedronCollider::CapsuleCollision(CapsuleCollider* collider)
{
	return false;
}


