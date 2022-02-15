#include "Framework.h"

Collider::Collider() : mbIsRender(true)
{
    mMaterial = new Material(L"Collider");
    mMaterial->GetBuffer()->data.diffuse = Float4(0, 1, 0, 1);
}

Collider::~Collider()
{
    delete mMaterial;
    delete mMesh;
}

bool Collider::Collision(Collider* collider)
{    
    switch (collider->mType)
    {
    case eType::BOX:
        return BoxCollision(static_cast<BoxCollider*>(collider));
        break;
    case eType::SPHERE:
        return SphereCollision(static_cast<SphereCollider*>(collider));
        break;
    case eType::CAPSULE:
        return CapsuleCollision(static_cast<CapsuleCollider*>(collider));
        break;    
    }

    return false;
}

void Collider::Update()
{
    if (KEY_DOWN(VK_F3))
    {
        if (mbIsRender)
        {
            SetColor({ 0, 1, 0, 1 });
            mbIsRender = !mbIsRender;
        }
        else
        {
            SetColor({ 0,0,0,0 });
            mbIsRender = !mbIsRender;
        }
    }

    UpdateWorld();
}

void Collider::PreRenderForColorPicking()
{
    mMaterial->SetShader(L"ColorPicking");
    Transform::SetWorldBuffer();
    Transform::SetColorBuffer();

    //WORLDCAMERA->SetVertexShader();

    mMeshForColorPicking->IASet(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    mMaterial->Set();

   // Environment::Get()->SetOrthographicProjectionBuffer();
    DEVICECONTEXT->DrawIndexed(mIndicesForColorPicking.size(), 0, 0);
   // Environment::Get()->SetPerspectiveProjectionBuffer();
}

void Collider::Render()
{
    mMaterial->SetShader(L"Collider");
    Transform::SetWorldBuffer(); // Set WorldMatrix to VertexShader.
   // WORLDCAMERA->SetVertexShader();

    //WORLDCAMERA->UpdateWorld();

    //Vector3 focus = WORLDCAMERA->mPosition + WORLDCAMERA->Forward();
    //Matrix viewMatrix;
    //Matrix invMatrix;
    //ViewBuffer* viewBuffer = new ViewBuffer;
    //viewMatrix = XMMatrixLookAtLH(WORLDCAMERA->mPosition.data, focus.data, WORLDCAMERA->Up().data); // ī�޶���ġ, Ÿ����ġ, ī�޶� ������
    //viewMatrix = XMMatrixIdentity();

    //viewBuffer->Set(viewMatrix);
    //viewBuffer->SetVSBuffer(1);


    mMesh->IASet(D3D_PRIMITIVE_TOPOLOGY_LINELIST);    
    mMaterial->Set();

    //Environment::Get()->SetOrthographicProjectionBuffer();
    DEVICECONTEXT->DrawIndexed(mIndices.size(), 0, 0);
   // Environment::Get()->SetPerspectiveProjectionBuffer();
}