#include "Framework.h"
#include "DeferredRenderingScene.h"

DeferredRenderingScene::DeferredRenderingScene()
{

	CreateModels(); // 모델들 디폴트셰이더는 Lighting

	gBuffer = new GBuffer(); // rtv생성

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
	gBuffer->PreRender(); // 여기서 OM에 Set. (rtv 3개, dev 1개)

	bunny->GetModel()->SetShader(L"GBuffer");
	groot->SetShader(L"GBuffer");
	sphere->GetMaterial()->SetShader(L"GBuffer");
	plane->GetModel()->SetShader(L"GBuffer");
	// 디퍼드라이팅셰이더에서 쓸 텍스쳐들 4개 생성.
	// GBuffer의 픽셀셰이더에서 OM의 렌더타겟에 float4형태로 리턴시켜놨음.


	bunny->Render(); // 각 렌더에서 렌더링파이프라인 호출함.
	groot->Render();
	sphere->Render();
	plane->Render();

	// Device::Clear() // 디바이스꺼 RTV,DSV 클리어 하고 렌더로 넘어감.
}

void DeferredRenderingScene::Render()
{
	// Device::SetRenderTarget

	TARGETCAMERA->GetViewBuffer()->SetPSBuffer(3); // 카메라 뷰버퍼 PS 3번에 셋
	Environment::Get()->GetProjectionBuffer()->SetPSBuffer(2);

	gBuffer->Render(); // OM에 셋팅되어있는 RTV들의 SRV를 픽셀셰이더에 Set. 디퍼드라이팅셰이더에서 사용할거임.


	// 최종화면 렌더셋팅.
	vertexBuffer->IASet(); // 디폴트 0번.
	DEVICECONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	material->Set(); // 디퍼드라이팅셰이더파일 Set. 이거 Set하고 Draw했으니 딱 맞다.

	DEVICECONTEXT->Draw(4, 0);
}

void DeferredRenderingScene::PostRender()
{
	gBuffer->PostRender(); // UIImage들 렌더.
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

