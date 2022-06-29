#include "Framework.h"

BoxCollider::BoxCollider(Vector3 minBox, Vector3 maxBox)
    : mMinBox(minBox), mMaxBox(maxBox)
{
    mType = eColliderType::BOX;
    createMesh();
    createMeshForColorPicking();
}

BoxCollider::~BoxCollider()
{
}

bool BoxCollider::RayCollision(IN Ray ray, OUT Contact* contact)
{
    if (ray.direction.Length() == 0.0f)
        return false;

    Contact temp;
    temp.distance = FLT_MAX;

    UINT face[] = {
        0, 1, 2, 3,//F
        4, 5, 6, 7,//B
        0, 1, 5, 4,//L
        1, 5, 6, 2,//R
        2, 3, 7, 6,//U
        0, 3, 7, 4,//D
    };

    for (UINT i = 0; i < 6; i++)
    {
        Vector3 p[4];

        p[0] = mVertices[face[i * 4 + 0]].position;
        p[1] = mVertices[face[i * 4 + 1]].position;
        p[2] = mVertices[face[i * 4 + 2]].position;
        p[3] = mVertices[face[i * 4 + 3]].position;

        p[0] = XMVector3TransformCoord(p[0].data, mWorldMatrix);
        p[1] = XMVector3TransformCoord(p[1].data, mWorldMatrix);
        p[2] = XMVector3TransformCoord(p[2].data, mWorldMatrix);
        p[3] = XMVector3TransformCoord(p[3].data, mWorldMatrix);

        float dist;
        if (Intersects(ray.position.data, ray.direction.data,
            p[0].data, p[1].data, p[2].data, dist))
        {
            if (dist < temp.distance)
            {
                temp.distance = dist;
                temp.hitPoint = ray.position + ray.direction * dist;
            }
        }

        if (Intersects(ray.position.data, ray.direction.data,
            p[0].data, p[3].data, p[2].data, dist))
        {
            if (dist < temp.distance)
            {
                temp.distance = dist;
                temp.hitPoint = ray.position + ray.direction * dist;
            }
        }
    }

    if (temp.distance == FLT_MAX)
        return false;

    if (contact != nullptr)
    {
        contact->distance = temp.distance;
        contact->hitPoint = temp.hitPoint;
    }

    return true;
}


bool BoxCollider::BoxCollision(BoxCollider* collider)
{
    OBB box1 = GetObb();
    OBB box2 = collider->GetObb();

    Vector3 D = box2.position - box1.position;

    for(UINT i = 0; i < 3; i++)
    {
        if (seperateAxis(D, box1.axis[i], box1, box2)) return false;
        if (seperateAxis(D, box2.axis[i], box1, box2)) return false;
    }

    for (UINT i = 0; i < 3; i++)
    {
        for (UINT j = 0; j < 3; j++)
        {
            if (box1.axis[i] == box2.axis[j]) return true;
        }
    }

    for (UINT i = 0; i < 3; i++)
    {
        for (UINT j = 0; j < 3; j++)
        {
            Vector3 cross = Vector3::Cross(box1.axis[i], box2.axis[j]);
            if (seperateAxis(D, cross, box1, box2)) return false;
        }
    }

    return true;
}

bool BoxCollider::SphereCollision(SphereCollider* collider)
{
    Matrix T = XMMatrixTranslation(GetGlobalPosition().x, GetGlobalPosition().y, GetGlobalPosition().z);
    Matrix R = XMMatrixRotationQuaternion(GetGlobalRotation().data);

    Matrix invWorld = XMMatrixInverse(nullptr, R * T);

    Vector3 spherePos = XMVector3TransformCoord(collider->GetGlobalPosition().data, invWorld);

    Vector3 tempMin = mMinBox * GetGlobalScale();
    Vector3 tempMax = mMaxBox * GetGlobalScale();

    Vector3 temp;
    temp.x = max(tempMin.x, min(spherePos.x, tempMax.x));
    temp.y = max(tempMin.y, min(spherePos.y, tempMax.y));
    temp.z = max(tempMin.z, min(spherePos.z, tempMax.z));

    temp -= spherePos;

    return temp.Length() <= collider->Radius();
}


bool BoxCollider::CapsuleCollision(CapsuleCollider* collider)
{
    return collider->BoxCollision(this);
}

void BoxCollider::createMeshForColorPicking()
{
    mVerticesForColorPicking.resize(24);

    //Front
    mVerticesForColorPicking[0].position = { mMinBox.x, mMinBox.y, mMinBox.z };
    mVerticesForColorPicking[1].position = { mMinBox.x, mMaxBox.y, mMinBox.z };
    mVerticesForColorPicking[2].position = { mMaxBox.x, mMaxBox.y, mMinBox.z };
    mVerticesForColorPicking[3].position = { mMaxBox.x, mMinBox.y , mMinBox.z };

    //Back
    mVerticesForColorPicking[4].position = { mMinBox.x, mMinBox.y, mMaxBox.z };
    mVerticesForColorPicking[5].position = { mMinBox.x, mMaxBox.y, mMaxBox.z };
    mVerticesForColorPicking[6].position = { mMaxBox.x, mMaxBox.y, mMaxBox.z };
    mVerticesForColorPicking[7].position = { mMaxBox.x, mMinBox.y, mMaxBox.z };

    //Right
    mVerticesForColorPicking[8].position = { mMaxBox.x, mMinBox.y, mMinBox.z };
    mVerticesForColorPicking[9].position = { mMaxBox.x, mMaxBox.y, mMinBox.z };
    mVerticesForColorPicking[10].position = { mMaxBox.x, mMaxBox.y, mMaxBox.z };
    mVerticesForColorPicking[11].position = { mMaxBox.x, mMinBox.y, mMaxBox.z };

    //Left
    mVerticesForColorPicking[12].position = { mMinBox.x, mMinBox.y, mMinBox.z };
    mVerticesForColorPicking[13].position = { mMinBox.x, mMaxBox.y, mMinBox.z };
    mVerticesForColorPicking[14].position = { mMinBox.x, mMaxBox.y, mMaxBox.z };
    mVerticesForColorPicking[15].position = { mMinBox.x, mMinBox.y, mMaxBox.z };

    //Up
    mVerticesForColorPicking[16].position = { mMinBox.x, mMaxBox.y, mMinBox.z };
    mVerticesForColorPicking[17].position = { mMinBox.x, mMaxBox.y, mMaxBox.z };
    mVerticesForColorPicking[18].position = { mMaxBox.x, mMaxBox.y, mMaxBox.z };
    mVerticesForColorPicking[19].position = { mMaxBox.x, mMaxBox.y, mMinBox.z };

    //Down
    mVerticesForColorPicking[20].position = { mMinBox.x, mMinBox.y, mMinBox.z };
    mVerticesForColorPicking[21].position = { mMinBox.x, mMinBox.y, mMaxBox.z };
    mVerticesForColorPicking[22].position = { mMaxBox.x, mMinBox.y, mMaxBox.z };
    mVerticesForColorPicking[23].position = { mMaxBox.x, mMinBox.y, mMinBox.z };

    /*for (UINT i = 0; i < 6; i++) // uv값이 필요....하지않을거다 아마. 텍스쳐를 바를건 아니니까.
    {
        mVertices[i * 4 + 0].uv = { 0, 1 };
        mVertices[i * 4 + 1].uv = { 0, 0 };
        mVertices[i * 4 + 2].uv = { 1, 0 };
        mVertices[i * 4 + 3].uv = { 1, 1 };
    }*/

    //Front
    mIndicesForColorPicking.emplace_back(0);
    mIndicesForColorPicking.emplace_back(1);
    mIndicesForColorPicking.emplace_back(2);

    mIndicesForColorPicking.emplace_back(0);
    mIndicesForColorPicking.emplace_back(2);
    mIndicesForColorPicking.emplace_back(3);

    //Back
    mIndicesForColorPicking.emplace_back(4);
    mIndicesForColorPicking.emplace_back(6);
    mIndicesForColorPicking.emplace_back(5);

    mIndicesForColorPicking.emplace_back(4);
    mIndicesForColorPicking.emplace_back(7);
    mIndicesForColorPicking.emplace_back(6);

    //Right
    mIndicesForColorPicking.emplace_back(8);
    mIndicesForColorPicking.emplace_back(9);
    mIndicesForColorPicking.emplace_back(10);

    mIndicesForColorPicking.emplace_back(8);
    mIndicesForColorPicking.emplace_back(10);
    mIndicesForColorPicking.emplace_back(11);

    //Left
    mIndicesForColorPicking.emplace_back(12);
    mIndicesForColorPicking.emplace_back(14);
    mIndicesForColorPicking.emplace_back(13);

    mIndicesForColorPicking.emplace_back(12);
    mIndicesForColorPicking.emplace_back(15);
    mIndicesForColorPicking.emplace_back(14);

    //Up
    mIndicesForColorPicking.emplace_back(16);
    mIndicesForColorPicking.emplace_back(17);
    mIndicesForColorPicking.emplace_back(18);

    mIndicesForColorPicking.emplace_back(16);
    mIndicesForColorPicking.emplace_back(18);
    mIndicesForColorPicking.emplace_back(19);

    //Down
    mIndicesForColorPicking.emplace_back(20);
    mIndicesForColorPicking.emplace_back(22);
    mIndicesForColorPicking.emplace_back(21);

    mIndicesForColorPicking.emplace_back(20);
    mIndicesForColorPicking.emplace_back(23);
    mIndicesForColorPicking.emplace_back(22);

    mMeshForColorPicking = new Mesh(mVerticesForColorPicking.data(), sizeof(Vertex), (UINT)mVerticesForColorPicking.size(),
        mIndicesForColorPicking.data(), (UINT)mIndicesForColorPicking.size());
}

bool BoxCollider::SphereCollision(Vector3 center, float radius)
{
    Matrix T = XMMatrixTranslation(GetGlobalPosition().x, GetGlobalPosition().y, GetGlobalPosition().z);
    Matrix R = XMMatrixRotationQuaternion(GetGlobalRotation().data);

    Matrix invWorld = XMMatrixInverse(nullptr, R * T);

    Vector3 spherePos = XMVector3TransformCoord(center.data, invWorld);

    Vector3 tempMin = mMinBox * GetGlobalScale();
    Vector3 tempMax = mMaxBox * GetGlobalScale();

    Vector3 temp;
    temp.x = max(tempMin.x, min(spherePos.x, tempMax.x));
    temp.y = max(tempMin.y, min(spherePos.y, tempMax.y));
    temp.z = max(tempMin.z, min(spherePos.z, tempMax.z));

    temp -= spherePos;

    return temp.Length() <= radius;
}

Vector3 BoxCollider::MinBox()
{
    return XMVector3TransformCoord(mMinBox.data, mWorldMatrix);
}

Vector3 BoxCollider::MaxBox()
{
    return XMVector3TransformCoord(mMaxBox.data, mWorldMatrix);
}

OBB BoxCollider::GetObb()
{
    mOBB.position = GetGlobalPosition();

    mOBB.axis[0] = GetRightVector();
    mOBB.axis[1] = GetUpVector();
    mOBB.axis[2] = GetForwardVector();

    mOBB.halfSize = (mMinBox - mMaxBox) * 0.5f * GetGlobalScale();

    return mOBB;
}

void BoxCollider::createMesh()
{
    mVertices.emplace_back(mMinBox.x, mMinBox.y, mMinBox.z);
    mVertices.emplace_back(mMinBox.x, mMaxBox.y, mMinBox.z);
    mVertices.emplace_back(mMaxBox.x, mMaxBox.y, mMinBox.z);
    mVertices.emplace_back(mMaxBox.x, mMinBox.y, mMinBox.z);

    mVertices.emplace_back(mMinBox.x, mMinBox.y, mMaxBox.z);
    mVertices.emplace_back(mMinBox.x, mMaxBox.y, mMaxBox.z);
    mVertices.emplace_back(mMaxBox.x, mMaxBox.y, mMaxBox.z);
    mVertices.emplace_back(mMaxBox.x, mMinBox.y, mMaxBox.z);

    mIndices = {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7
    };

    mMesh = new Mesh(mVertices.data(), sizeof(Vertex), mVertices.size(),
        mIndices.data(), mIndices.size());
}

bool BoxCollider::seperateAxis(Vector3 D, Vector3 axis, OBB box1, OBB box2)
{
    float distance = abs(Vector3::Dot(D, axis));

    float a = 0.0f;
    float b = 0.0f;

    for (UINT i = 0; i < 3; i++)
    {
        a += abs(Vector3::Dot(box1.axis[i] * box1.halfSize[i], axis));
        b += abs(Vector3::Dot(box2.axis[i] * box2.halfSize[i], axis));
    }

    return distance > a + b;
}
