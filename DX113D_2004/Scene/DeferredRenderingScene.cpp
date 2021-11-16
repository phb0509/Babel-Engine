#include "Framework.h"
#include "DeferredRenderingScene.h"

DeferredRenderingScene::DeferredRenderingScene()
{

	CreateModels(); // �𵨵� ����Ʈ���̴��� Lighting

	gBuffer = new GBuffer(); // rtv����

	material = new Material(L"DeferredLighting");

	CreateMesh();
}

DeferredRenderingScene::~DeferredRenderingScene()
{
	delete bunny;
	delete plane;
	delete groot;
	delete sphere;

	delete gBuffer;

	delete material;
	delete vertexBuffer;
}

void DeferredRenderingScene::Update()
{
	groot->Update();
	sphere->Update();
}

void DeferredRenderingScene::PreRender()
{
	gBuffer->PreRender(); // ���⼭ OM�� Set. (rtv 3��, dev 1��)

	bunny->GetModel()->SetShader(L"GBuffer");
	groot->SetShader(L"GBuffer");
	sphere->GetMaterial()->SetShader(L"GBuffer");
	plane->GetModel()->SetShader(L"GBuffer");
	// ���۵�����ü��̴����� �� �ؽ��ĵ� 4�� ����.
	// GBuffer�� �ȼ����̴����� OM�� ����Ÿ�ٿ� float4���·� ���Ͻ��ѳ���.


	bunny->Render(); // �� �������� ���������������� ȣ����.
	groot->Render();
	sphere->Render();
	plane->Render();

	// Device::Clear() // ����̽��� RTV,DSV Ŭ���� �ϰ� ������ �Ѿ.
}

void DeferredRenderingScene::Render()
{
	// Device::SetRenderTarget

	TARGETCAMERA->GetViewBuffer()->SetPSBuffer(3); // ī�޶� ����� PS 3���� ��
	Environment::Get()->GetProjectionBuffer()->SetPSBuffer(2);

	gBuffer->Render(); // OM�� ���õǾ��ִ� RTV���� SRV�� �ȼ����̴��� Set. ���۵�����ü��̴����� ����Ұ���.


	// ����ȭ�� ��������.
	vertexBuffer->IASet(); // ����Ʈ 0��.
	DEVICECONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	material->Set(); // ���۵�����ü��̴����� Set. �̰� Set�ϰ� Draw������ �� �´�.

	DEVICECONTEXT->Draw(4, 0);
}

void DeferredRenderingScene::PostRender()
{
	gBuffer->PostRender(); // UIImage�� ����.
}

void DeferredRenderingScene::CreateModels()
{
	TARGETCAMERA->mPosition = { 0, 10, -20 };
	TARGETCAMERA->mRotation.x = 0.4f;

	bunny = new ModelObject("StanfordBunny/StanfordBunny", Collider::NONE);
	bunny->mScale = { 0.01f, 0.01f, 0.01f };
	bunny->mPosition.y = 2.35f;
	bunny->mPosition.x = -3.0f;

	plane = new ModelObject("Plane/Plane", Collider::NONE);
	plane->mScale = { 2.0f, 2.0f, 2.0f };

	groot = new ModelAnimObject("Groot/Groot");
	groot->mScale = { 0.01f, 0.01f, 0.01f };
	groot->mPosition.x = 3.0f;
	groot->SetShader(L"Lighting");
	groot->ReadClip("Groot/Dancing0");

	groot->SetDiffuseMap(L"ModelData/Materials/Groot/groot_diffuse.png");
	groot->SetSpecularMap(L"ModelData/Materials/Groot/groot_glossiness.png");
	groot->SetNormalMap(L"ModelData/Materials/Groot/groot_normal_map.png");

	groot->PlayClip(0);

	sphere = new Sphere(L"Lighting");
	sphere->mPosition = { 0.0f, 1.0f, -3.0f };
	sphere->GetMaterial()->SetDiffuseMap(L"Textures/Landscape/Fieldstone_DM.tga");
	sphere->GetMaterial()->SetNormalMap(L"Textures/Landscape/fieldstone_NM.tga");
}

void DeferredRenderingScene::CreateMesh()
{
	UINT vertices[4] = { 0, 1, 2, 3 };
	vertexBuffer = new VertexBuffer(vertices, sizeof(UINT), 4);
}

