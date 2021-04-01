#include "Framework.h"

Models::Models(string file) : Model(file), drawCount(0)
{  
    for (InstanceData& data : instanceData)
        data.world = XMMatrixIdentity();

    instanceBuffer = new VertexBuffer(instanceData, sizeof(InstanceData), MAX_INSTANCE);

    frustum = new Frustum();
    SetBox(&minBox, &maxBox);
}

Models::~Models()
{
    for (Transform* transform : transforms)
        delete transform;

    delete instanceBuffer;
    delete frustum;
}

void Models::Update()
{
    frustum->Update();
    drawCount = 0;

    for (UINT i = 0; i < transforms.size(); i++)
    {

        Vector3 worldMin = XMVector3TransformCoord(minBox.data, *transforms[i]->GetWorld());
        Vector3 worldMax = XMVector3TransformCoord(maxBox.data, *transforms[i]->GetWorld());
        if (frustum->ContainBox(worldMin, worldMax))
        {
            transforms[i]->UpdateWorld();
            instanceData[drawCount].world = XMMatrixTranspose(*transforms[i]->GetWorld());
            instanceData[drawCount].index = i;
            drawCount++;
        }
    }

    instanceBuffer->Update(instanceData, drawCount);
}

void Models::Render()
{
    instanceBuffer->IASet(1);
    SetBoneTransforms();
    boneBuffer->SetVSBuffer(3);

    MeshRender(drawCount);
}

Transform* Models::Add()
{
    Transform* transform = new Transform();
    transforms.emplace_back(transform);   

    return transform;
}
