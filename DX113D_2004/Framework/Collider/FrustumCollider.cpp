#include "Framework.h"



FrustumCollider::FrustumCollider(float nearRectWidth, float nearRectHeight, float farRectWidth, float farRectHeight, float distanceToNearRect, float distanceToFarRect):
    mNearRectWidth(nearRectWidth),
    mNearRectHeight(nearRectHeight),
    mFarRectWidth(farRectWidth),
    mFarRectHeight(farRectHeight),
    mDistanceToNearRect(distanceToNearRect),
    mDistanceToFarRect(distanceToFarRect)
{
    CreateMesh();
}

FrustumCollider::~FrustumCollider()
{
}

bool FrustumCollider::RayCollision(IN Ray ray, OUT Contact* contact)
{
    return false;
}


bool FrustumCollider::BoxCollision(BoxCollider* collider)
{
    return false;
}

bool FrustumCollider::SphereCollision(SphereCollider* collider)
{
    return false;
}

bool FrustumCollider::CapsuleCollision(CapsuleCollider* collider)
{
    return false;
}

void FrustumCollider::CreateMesh()
{
    mVertices.emplace_back(0.0f, 0.0f, 0.0f); // 원점

    // NearRect
    mVertices.emplace_back(-mNearRectWidth / 2.0f, -mNearRectHeight / 2.0f, mDistanceToNearRect);
    mVertices.emplace_back(-mNearRectWidth / 2.0f, mNearRectHeight / 2.0f, mDistanceToNearRect);
    mVertices.emplace_back(mNearRectWidth / 2.0f, mNearRectHeight / 2.0f, mDistanceToNearRect);
    mVertices.emplace_back(mNearRectWidth / 2.0f, -mNearRectHeight / 2.0f, mDistanceToNearRect);
    
    // FarRect
    mVertices.emplace_back(-mFarRectWidth / 2.0f, -mFarRectHeight / 2.0f, mDistanceToFarRect);
    mVertices.emplace_back(-mFarRectWidth / 2.0f, mFarRectHeight / 2.0f, mDistanceToFarRect);
    mVertices.emplace_back(mFarRectWidth / 2.0f, mFarRectHeight / 2.0f, mDistanceToFarRect);
    mVertices.emplace_back(mFarRectWidth / 2.0f, -mFarRectHeight / 2.0f, mDistanceToFarRect);

    mIndices = {
        1,2,2,3,3,4,4,1, // NearRect 이어주기
        5,6,6,7,7,8,8,5, // FarRect 이어주기
        1,5,2,6,3,7,4,8, // NearRect와 FarRect 이어주기
    };

    mMesh = new Mesh(mVertices.data(), sizeof(Vertex), mVertices.size(),
        mIndices.data(), mIndices.size());
}
