#include "Framework.h"

Models::Models() : 
    Model(), 
    drawCount(0),
    mTargetCameraFrustum(nullptr)
{
    for (InstanceData& data : instanceData)
        data.world = XMMatrixIdentity();

    instanceBuffer = new VertexBuffer(instanceData, sizeof(InstanceData), MAX_INSTANCE);

    mTargetCameraFrustum = Environment::Get()->GetTargetCamera()->GetFrustum();
    SetBox(&minBox, &maxBox);
}

Models::~Models()
{
    for (Transform* transform : transforms)
        delete transform;

    delete instanceBuffer;
    delete mTargetCameraFrustum;
}

void Models::Update()
{
    mTargetCameraFrustum->Update();
    drawCount = 0;

    for (UINT i = 0; i < transforms.size(); i++)
    {
        Vector3 worldMin = XMVector3TransformCoord(minBox.data, *transforms[i]->GetWorld());
        Vector3 worldMax = XMVector3TransformCoord(maxBox.data, *transforms[i]->GetWorld());

        if (mTargetCameraFrustum->ContainBox(worldMin, worldMax)) // �������ҹ������� �ν��Ͻ��鸸 ������.
        {
            transforms[i]->UpdateWorld();
            instanceData[drawCount].world = XMMatrixTranspose(*transforms[i]->GetWorld()); // ���̴��� �ѱ������ ��ġ��� ��ȯ.
            instanceData[drawCount].index = i;
            drawCount++;
        }
    }

    instanceBuffer->Update(instanceData, drawCount); // ��������ų��ŭ�� ���� �޸𸮸� ������Ʈ.(drawCount)
}

void Models::Render()
{
    instanceBuffer->IASet(1);
    SetBoneTransforms();
    mBoneBuffer->SetVSBuffer(3);

    MeshRender(drawCount);
}

Transform* Models::Add()
{
    Transform* transform = new Transform();
    transforms.emplace_back(transform);

    return transform;
}
