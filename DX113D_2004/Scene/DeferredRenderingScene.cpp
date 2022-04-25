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
	mSphere->GetMaterial()->SetDiffuseMap(L"Textures/Bricks.png");
	mSphere->GetMaterial()->SetNormalMap(L"Textures/Bricks_normal.png");
	

	mTerrain = new Terrain();
	mTerrain->SetCamera(mWorldCamera);
	mLightBuffer = new LightBuffer();

	mDirectionalLight = new Light(LightType::DIRECTIONAL);
	mDirectionalLight->mTag = "DirectionalLight";

	mPointLight = new Light(LightType::POINT);
	mPointLight->mTag = "PointLight";

	mSpotLight = new Light(LightType::SPOT);
	mSpotLight->mTag = "SpotLight";

	
	
	//mLightBuffer->Add(mDirectionalLight->GetLightInfo());
	mLightBuffer->Add(mDirectionalLight);
	//mLightBuffer->Add(mPointLight);
	//mLightBuffer->Add(mSpotLight);

	// Create Models
	mGroot = new ModelAnimObject();
	mGroot->mScale = { 0.01f, 0.01f, 0.01f };
	mGroot->mPosition.x = 20.0f;
	mGroot->SetShader(L"GBuffer");
	mGroot->SetMesh("Groot", "Groot.mesh");
	mGroot->SetMaterial("Groot", "Groot.mat");
	mGroot->ReadClip("Groot", "Dancing0.clip");

	mGroot->SetDiffuseMap(L"ModelData/Groot/groot_diffuse.png");
	mGroot->SetSpecularMap(L"ModelData/Groot/groot_glossiness.png");
	mGroot->SetNormalMap(L"ModelData/Groot/groot_normal_map.png");
	mGroot->UpdateWorld();

	mPlayer = new ModelAnimObject();
	mPlayer->mScale = { 0.05f, 0.05f, 0.05f };
	//mPlayer->mPosition.x = 0.0f;
	mPlayer->SetShader(L"GBuffer");
	mPlayer->SetMesh("Player", "Player.mesh");
	mPlayer->SetMaterial("Player", "Player.mat");
	mPlayer->ReadClip("Player", "Idle.clip");
	mPlayer->mPosition = { 10.0f,0.0f,10.0f };
	mPlayer->UpdateWorld();

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
	mDirectionalLight->Update();
	//mPointLight->Update();
	//mSpotLight->Update();
	mLightBuffer->Update();

	mWorldCamera->Update();
	moveWorldCamera();

	mTerrain->Update();
	mGroot->Update();
	mPlayer->Update();
	mSphere->Update();
}

void DeferredRenderingScene::PreRender()
{
	Environment::Get()->Set(); // SetViewPort
	mWorldCamera->SetViewBuffer(); // 1번 레지스터
	mWorldCamera->SetProjectionBuffer(); // 2번 레지스터
	
	mGBuffer->PreRender(); // 여기서 OM에 Set. (rtv 4개, dev 1개)

	// 디퍼드라이팅셰이더에서 쓸 텍스쳐들 4개 생성.
	mGroot->SetShader(L"GBuffer");
	mPlayer->SetShader(L"GBuffer");
	mDirectionalLight->GetSphere()->GetMaterial()->SetShader(L"GBuffer"); // 스피어 렌더하는거임.
	mSphere->GetMaterial()->SetShader(L"GBuffer");
	mTerrain->GetMaterial()->SetShader(L"GBuffer");
	// GBuffer의 픽셀셰이더에서 OM의 렌더타겟에 float4형태로 리턴시켜놨음.

	mGroot->DeferredRender();
	mPlayer->DeferredRender();
	mSphere->Render();
	mTerrain->Render();

	mDirectionalLight->Render();
	//mPointLight->Render();
	//mSpotLight->Render();
}

void DeferredRenderingScene::Render()
{
	Device::Get()->ClearRenderTargetView(Float4(0.18f, 0.18f, 0.25f, 1.0f));
	//Device::Get()->ClearRenderTargetView(Float4(1.0f, 1.0f,  1.0f, 1.0f));
	Device::Get()->ClearDepthStencilView();
	Device::Get()->SetRenderTarget();
	Environment::Get()->Set(); //
	mLightBuffer->SetPSBuffer(0);
	
	mWorldCamera->GetViewBuffer()->SetPSBuffer(3);
	mWorldCamera->GetProjectionBufferInUse()->SetPSBuffer(2);
	mGBuffer->SetRenderTargetsToPS(); // OM에 셋팅되어있는 RTV들의 SRV를 픽셀셰이더에 Set. 디퍼드라이팅셰이더에서 사용할거임.

	// 최종화면 렌더셋팅.
	mVertexBuffer->IASet(); // 디폴트 0번.
	DEVICECONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	mDeferredMaterial->Set(); // 디퍼드라이팅셰이더파일 Set. 이거 Set하고 Draw했으니 딱 맞다.
	DEVICECONTEXT->Draw(4, 0);
	mGBuffer->ClearSRVs();
}

void DeferredRenderingScene::PostRender()
{
	mGBuffer->PostRender(); // UIImage들 렌더.

	ImGui::Begin("Player");
	ImGui::InputFloat3("PlayerPosition", (float*)&mPlayer->mPosition);
	ImGui::InputFloat3("PlayerRotation", (float*)&mPlayer->mRotation);
	ImGui::InputFloat3("PlayerScale", (float*)&mPlayer->mScale);
	ImGui::End();

	mSphere->PostRender();

	mLightBuffer->PostRender();
	mDirectionalLight->PostRender();
	//mPointLight->PostRender();
	//mSpotLight->PostRender();
}



void DeferredRenderingScene::moveWorldCamera()
{
	// Update Position
	if (KEY_PRESS(VK_RBUTTON))
	{
		if (KEY_PRESS('I'))
			mWorldCamera->mPosition += mWorldCamera->Forward() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('K'))
			mWorldCamera->mPosition -= mWorldCamera->Forward() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('J'))
			mWorldCamera->mPosition -= mWorldCamera->Right() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('L'))
			mWorldCamera->mPosition += mWorldCamera->Right() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('U'))
			mWorldCamera->mPosition -= mWorldCamera->Up() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('O'))
			mWorldCamera->mPosition += mWorldCamera->Up() * mWorldCamera->mMoveSpeed * DELTA;
	}

	mWorldCamera->mPosition += mWorldCamera->Forward() * Control::Get()->GetWheel() * mWorldCamera->mWheelSpeed * DELTA;

	// Update Rotation
	if (KEY_PRESS(VK_RBUTTON))
	{
		Vector3 value = MOUSEPOS - mPreFrameMousePosition;

		mWorldCamera->mRotation.x += value.y * mWorldCamera->mRotationSpeed * DELTA;
		mWorldCamera->mRotation.y += value.x * mWorldCamera->mRotationSpeed * DELTA;
	}

	mPreFrameMousePosition = MOUSEPOS;
	mWorldCamera->SetViewMatrixToBuffer();
}

