#include "Framework.h"
#include "DeferredRenderingScene.h"

DeferredRenderingScene::DeferredRenderingScene()
{
	mWorldCamera = new Camera();
	mWorldCamera->mPosition = { 0, 10, -20 };
	mWorldCamera->mRotation.x = 0.4f;

	mSphere = new Sphere(L"Lighting");
	mSphere->mScale = { 3.0f,3.0f,3.0f };
	mSphere->mPosition = { 5.0f,3.0f,10.0f };
	mSphere->GetMaterial()->SetDiffuseMap(L"Textures/Wall.png");
	mSphere->GetMaterial()->SetNormalMap(L"Textures/Wall_normal.png");
	mSphere->GetMaterial()->SetSpecularMap(L"Textures/Wall_specular.png");
	

	mTerrain = new Terrain();
	mTerrain->SetCamera(mWorldCamera);
	mLightBuffer = new LightBuffer();

	mDirectionalLight = new Light(LightType::DIRECTIONAL);
	mDirectionalLight->mTag = "DirectionalLight";

	mPointLight = new Light(LightType::POINT);
	mPointLight->mTag = "PointLight"; 

	mSpotLight = new Light(LightType::SPOT);
	mSpotLight->mTag = "SpotLight";

	
	mLightBuffer->Add(mDirectionalLight);
	mLightBuffer->Add(mPointLight);
	mLightBuffer->Add(mSpotLight);

	// Create Models
	mGroot = new ModelAnimObject();
	mGroot->mScale = { 0.01f, 0.01f, 0.01f };
	mGroot->mPosition.x = 0.0f;
	mGroot->SetShader(L"GBuffer");
	mGroot->SetMesh("Groot", "Groot.mesh");
	mGroot->SetMaterial("Groot", "Groot.mat");

	mGroot->SetDiffuseMap(L"ModelData/Groot/groot_diffuse.png");
	mGroot->SetNormalMap(L"ModelData/Groot/groot_normal_map.png");
	mGroot->SetSpecularMap(L"ModelData/Groot/groot_glossiness.png");
	mGroot->ReadClip("Groot", "Dancing0.clip");
	mGroot->UpdateWorld();

	mPlayer = new ModelAnimObject();
	mPlayer->mScale = { 0.05f, 0.05f, 0.05f };
	//mPlayer->mPosition.x = 0.0f;
	mPlayer->SetShader(L"GBuffer");
	mPlayer->SetMesh("Player", "Player.mesh");
	mPlayer->SetMaterial("Player", "Player.mat");
	/*mPlayer->SetDiffuseMap(L"ModelData/Player/Paladin_diffuse.png");
	mPlayer->SetNormalMap(L"ModelData/Player/Paladin_normal.png");
	mPlayer->SetSpecularMap(L"ModelData/Player/Paladin_specular.png");*/
	mPlayer->ReadClip("Player", "Idle.clip");
	mPlayer->mPosition = { 10.0f,0.0f,0.0f };
	mPlayer->UpdateWorld();

	mMutant = new ModelAnimObject();
	mMutant->mScale = { 0.05f, 0.05f, 0.05f };
	
	mMutant->SetShader(L"GBuffer");
	mMutant->SetMesh("Mutant", "Mutant.mesh");
	mMutant->SetMaterial("Mutant", "Mutant.mat");
	mMutant->ReadClip("Mutant", "Idle.clip");
	mMutant->mPosition = { 20.0f,0.0f,0.0f };
	mMutant->UpdateWorld();

	mGBuffer = new GBuffer(); // rtv생성

	mDeferredMaterial = new Material(L"DeferredLighting");

	UINT vertices[4] = { 0, 1, 2, 3 };
	mVertexBuffer = new VertexBuffer(vertices, sizeof(UINT), 4);
}

DeferredRenderingScene::~DeferredRenderingScene()
{
	delete mGroot;
	delete mGBuffer;
	delete mDeferredMaterial;
	delete mVertexBuffer;
	delete mLightBuffer;
	delete mDirectionalLight;
}

void DeferredRenderingScene::Update()
{
	mLightBuffer->Update();
	mDirectionalLight->Update();
	mPointLight->Update();
	mSpotLight->Update();

	mWorldCamera->Update();
	mWorldCamera->Move();

	mTerrain->Update();
	mSphere->Update();
	mGroot->Update();
	mPlayer->Update();
	mMutant->Update();

	if (KEY_PRESS('Q'))
	{
		mPlayer->StopAnimation();
	}
	if (KEY_PRESS('W'))
	{
		mPlayer->PlayAnimation();
	}

	if (KEY_PRESS('E'))
	{
		mGroot->StopAnimation();
	}
	if (KEY_PRESS('R'))
	{
		mGroot->PlayAnimation();
	}
}

void DeferredRenderingScene::PreRender()
{
	Environment::Get()->Set(); // SetViewPort
	mWorldCamera->SetViewBufferToVS(); // 1번 레지스터
	mWorldCamera->SetProjectionBufferToVS(); // 2번 레지스터
	
	mGBuffer->PreRender(); // 여기서 OM에 Set. (rtv 4개, dev 1개)

	// 디퍼드라이팅셰이더에서 쓸 텍스쳐들 4개 생성.
	mGroot->SetShader(L"GBuffer");
	mPlayer->SetShader(L"GBuffer");
	mMutant->SetShader(L"GBuffer");
	mDirectionalLight->GetSphere()->GetMaterial()->SetShader(L"GBuffer"); // Render Sphere
	mPointLight->GetSphere()->GetMaterial()->SetShader(L"GBuffer"); // Render Sphere
	mSpotLight->GetSphere()->GetMaterial()->SetShader(L"GBuffer"); // Render Sphere
	mSphere->GetMaterial()->SetShader(L"GBuffer");
	mTerrain->GetMaterial()->SetShader(L"GBuffer");

	mGroot->DeferredRender();
	mPlayer->DeferredRender();
	mMutant->DeferredRender();

	mSphere->Render();
	mTerrain->Render();

	//mDirectionalLight->Render();
}

void DeferredRenderingScene::Render()
{
	Device::Get()->ClearRenderTargetView(Float4(0.18f, 0.18f, 0.25f, 1.0f));
	Device::Get()->ClearDepthStencilView();
	Device::Get()->SetRenderTarget();
	Environment::Get()->Set(); //
	mWorldCamera->SetViewBufferToPS(10); 
	mWorldCamera->SetProjectionBufferToPS(11); 

	mLightBuffer->SetPSBuffer(0);
	
	//mWorldCamera->GetViewBuffer()->SetPSBuffer(3);
	//mWorldCamera->GetProjectionBufferInUse()->SetPSBuffer(2);
	mGBuffer->SetRenderTargetsToPS(); // OM에 셋팅되어있는 RTV들의 SRV를 픽셀셰이더에 Set. 디퍼드라이팅셰이더에서 사용할거임.

	// 최종화면 렌더셋팅.
	mVertexBuffer->IASet(); // 디폴트 0번.
	DEVICECONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	mDeferredMaterial->Set(); // 디퍼드라이팅셰이더파일 Set. 이거 Set하고 Draw했으니 딱 맞다.
	DEVICECONTEXT->Draw(4, 0);
	mGBuffer->ClearSRVs();
}

static bool	hasPlayerSpecularMap = true;

void DeferredRenderingScene::PostRender()
{
	mGBuffer->PostRender(); // UIImage들 렌더.

	ImGui::Begin("Player");
	ImGui::InputFloat3("PlayerPosition", (float*)&mPlayer->mPosition);
	ImGui::InputFloat3("PlayerRotation", (float*)&mPlayer->mRotation);
	ImGui::InputFloat3("PlayerScale", (float*)&mPlayer->mScale);
	ImGui::Checkbox("HasSpecularMap", (bool*)&hasPlayerSpecularMap);
	ImGui::End();

	mPlayer->IsRenderSpecularMap(hasPlayerSpecularMap);

	mSphere->PostRender();
	mLightBuffer->PostRender();
	mDirectionalLight->PostRender();
	mPointLight->PostRender();
	mSpotLight->PostRender();
}





