#include "Framework.h"

Collider::Collider() : mbIsRender(true)
{
    mMaterial = new Material(L"Collider");
    mMaterial->GetBuffer()->data.diffuse = Float4(0.0f, 1.0f, 0.0f, 1.0f);

}

Collider::~Collider()
{
    GM->SafeDelete(mMaterial);
    GM->SafeDelete(mMesh);
}

bool Collider::Collision(Collider* collider)
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

    mMeshForColorPicking->IASet(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    DEVICECONTEXT->DrawIndexed(mIndicesForColorPicking.size(), 0, 0);
}

void Collider::Render()
{
    mMaterial->SetShader(L"Collider");
    mMaterial->Set();
    //SetColor({ 0,1,0,0 });

    //Transform::UpdateWorld();
    Transform::SetWorldBuffer(); // Set WorldMatrix to VertexShader.

    mMesh->IASet(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

    DEVICECONTEXT->DrawIndexed(mIndices.size(), 0, 0);
}