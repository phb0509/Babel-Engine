#include "Framework.h"

Billboard::Billboard(wstring diffuseFile)
{
	mMaterial = new Material(L"Texture");
	mMaterial->SetDiffuseMap(diffuseFile);

	createMesh();
}

Billboard::~Billboard()
{
	delete mMaterial;
	delete mMesh;
}

void Billboard::Update()
{
	Vector3 dir = mPosition - mCamera->mPosition; // 오브젝트가 카메라를 바라보는 방향벡터.
	mRotation.y = atan2(dir.x, dir.z);
}

void Billboard::Render()
{
	SetWorldBuffer();
	mMesh->SetIA();
	mMaterial->Set();

	DEVICECONTEXT->DrawIndexed(6, 0, 0);
}

void Billboard::createMesh()
{
	VertexUV vertices[4];
	vertices[0].position = Float3(-0.5f, 0.5f, 0.0f);
	vertices[1].position = Float3(0.5f, 0.5f, 0.0f);
	vertices[2].position = Float3(-0.5f, -0.5f, 0.0f);
	vertices[3].position = Float3(0.5f, -0.5f, 0.0f);

	vertices[0].uv = Float2(0, 0);
	vertices[1].uv = Float2(1, 0);
	vertices[2].uv = Float2(0, 1);
	vertices[3].uv = Float2(1, 1);

	UINT indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	mMesh = new Mesh(vertices, sizeof(VertexUV), 4, indices, 6);
}
