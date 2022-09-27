#include "Framework.h"

Collider::Collider() : mbIsRender(true)
{
    mMaterial = new Material(L"Collider");
    mMaterial->GetBuffer()->data.diffuse = Float4(0.0f, 1.0f, 0.0f, 1.0f);
    mTypeBuffer = new TypeBuffer();
    mTypeBuffer->data.values[0] = 4;
}

Collider::~Collider()
{
    GM->SafeDelete(mMaterial);
    GM->SafeDelete(mMesh);
}

bool Collider::Collision(Collider* collider) // ¸Å°³º¯¼ö´Â Ä®. Áö±Ý this´Â Ä¸½¶
{    
    switch (collider->mType)
    {
    case eColliderType::BOX:
        return BoxCollision(static_cast<BoxCollider*>(collider));
        break;
    case eColliderType::SPHERE:
        return SphereCollision(static_cast<SphereCollider*>(collider));
        break;
    case eColliderType::CAPSULE:
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
            SetColor({ 0, 1, 0, 0 });
            mbIsRender = !mbIsRender;
        }
        else
        {
            SetColor({ 0,0,0,0 });
            mbIsRender = !mbIsRender;
        }
    }

    Transform::UpdateWorld();
}

void Collider::PreRenderForColorPicking()
{
    mMaterial->SetShader(L"ColorPicking");
    mMaterial->Set();

    Transform::SetWorldBuffer();
    Transform::SetHashColorBuffer();

    mMeshForColorPicking->SetIA(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    DEVICECONTEXT->DrawIndexed(mIndicesForColorPicking.size(), 0, 0);
}

void Collider::Render()
{
    if (!DM->GetIsDebugMode())
    {
        mMaterial->SetShader(L"DeferredCollider");
        mMaterial->Set();

        //Transform::UpdateWorld();
        Transform::SetWorldBuffer(); // Set WorldMatrix to VertexShader.
        mMesh->SetIA(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

        DEVICECONTEXT->DrawIndexed(mIndices.size(), 0, 0);
    }
}

void Collider::DeferredRender()
{
    if (!DM->GetIsDebugMode())
    {
        mMaterial->SetShader(L"GBuffer");
        mMaterial->Set();

        //Transform::UpdateWorld();
        Transform::SetWorldBuffer(); // Set WorldMatrix to VertexShader.

        mMesh->SetIA(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

        DEVICECONTEXT->DrawIndexed(mIndices.size(), 0, 0);
    }
}
