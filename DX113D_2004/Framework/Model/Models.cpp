#include "Framework.h"

Models::Models() : 
    Model(), 
    mDrawCount(0),
    mCameraForFrustumCulling(nullptr)
{
    for (InstanceData& data : mInstanceData)
        data.world = XMMatrixIdentity();

    mInstanceBuffer = new VertexBuffer(mInstanceData, sizeof(InstanceData), MAX_INSTANCE);
}

Models::~Models()
{
    for (Transform* transform : mTransforms)
        delete transform;

    delete mInstanceBuffer;
    delete mCameraForFrustumCulling;
}

void Models::Update()
{
    mCameraForFrustumCulling->Update();
    mDrawCount = 0;

    for (UINT i = 0; i < mTransforms.size(); i++)
    {
        Vector3 worldMin = XMVector3TransformCoord(mMinBox.data, *mTransforms[i]->GetWorldMatrix());
        Vector3 worldMax = XMVector3TransformCoord(mMaxBox.data, *mTransforms[i]->GetWorldMatrix());

        if (mCameraForFrustumCulling->GetFrustum()->ContainBox(worldMin, worldMax)) // �������ҹ������� �ν��Ͻ��鸸 ������.
        {
            mTransforms[i]->UpdateWorld();
            mInstanceData[mDrawCount].world = XMMatrixTranspose(*mTransforms[i]->GetWorldMatrix()); // ���̴��� �ѱ������ ��ġ��� ��ȯ.
            mInstanceData[mDrawCount].index = i;
            mDrawCount++;
        }
    }

    mInstanceBuffer->Update(mInstanceData, mDrawCount); // ��������ų��ŭ�� ���� �޸𸮸� ������Ʈ.(drawCount)
}

void Models::Render()
{
    mInstanceBuffer->IASet(1);
    SetBoneTransforms();
    mBoneBuffer->SetVSBuffer(3);

    MeshRender(mDrawCount);
}

Transform* Models::Add()
{
    Transform* transform = new Transform();
    mTransforms.emplace_back(transform);

    return transform;
}
