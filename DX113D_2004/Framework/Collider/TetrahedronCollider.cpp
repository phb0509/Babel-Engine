#include "Framework.h"

TetrahedronCollider::TetrahedronCollider(float rectSize, float distanceToRect) :
	mRectSize(rectSize),
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
	vertices.emplace_back(-mRectSize / 2.0f, -mRectSize / 2.0f, mDistanceToRect);
	vertices.emplace_back(-mRectSize / 2.0f, mRectSize / 2.0f, mDistanceToRect);
	vertices.emplace_back(mRectSize / 2.0f, mRectSize / 2.0f, mDistanceToRect);
	vertices.emplace_back(mRectSize / 2.0f, -mRectSize / 2.0f, mDistanceToRect);


	//indices = {
	//	0, 1, 1, 2, 2, 3, 3, 0,
	//	4, 5, 5, 6, 6, 7, 7, 4,
	//	0, 4, 1, 5, 2, 6, 3, 7
	//};

	indices = {
		1,2,2,3,3,4,4,1, // �� �̾��ֱ�
		0,1,0,2,0,3,0,4  // �������� �� �̾��ֱ�.
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

