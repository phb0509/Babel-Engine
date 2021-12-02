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
	mVertices.emplace_back(0.0f, 0.0f, 0.0f);
	mVertices.emplace_back(-mRectWidth / 2.0f, -mRectHeight / 2.0f, mDistanceToRect);
	mVertices.emplace_back(-mRectWidth / 2.0f, mRectHeight / 2.0f, mDistanceToRect);
	mVertices.emplace_back(mRectWidth / 2.0f, mRectHeight / 2.0f, mDistanceToRect);
	mVertices.emplace_back(mRectWidth / 2.0f, -mRectHeight / 2.0f, mDistanceToRect);

	mIndices = {
		1,2,2,3,3,4,4,1, // 면 이어주기
		0,1,0,2,0,3,0,4  // 꼭지점과 면 이어주기.
	};

	mMesh = new Mesh(mVertices.data(), sizeof(Vertex), mVertices.size(),
		mIndices.data(), mIndices.size());
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


