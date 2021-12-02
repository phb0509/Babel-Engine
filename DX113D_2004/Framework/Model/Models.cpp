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

        if (mTargetCameraFrustum->ContainBox(worldMin, worldMax)) // 프러스텀범위안의 인스턴스들만 렌더링.
        {
            transforms[i]->UpdateWorld();
            instanceData[drawCount].world = XMMatrixTranspose(*transforms[i]->GetWorld()); // 셰이더에 넘기기위해 전치행렬 변환.
            instanceData[drawCount].index = i;
            drawCount++;
        }
    }

    instanceBuffer->Update(instanceData, drawCount); // 렌더링시킬만큼의 수의 메모리만 업데이트.(drawCount)
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
