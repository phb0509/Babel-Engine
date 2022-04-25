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

	mGBuffer = new GBuffer(); // rtv����

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
	mWorldCamera->SetViewBuffer(); // 1�� ��������
	mWorldCamera->SetProjectionBuffer(); // 2�� ��������
	
	mGBuffer->PreRender(); // ���⼭ OM�� Set. (rtv 4��, dev 1��)

	// ���۵�����ü��̴����� �� �ؽ��ĵ� 4�� ����.
	mGroot->SetShader(L"GBuffer");
	mPlayer->SetShader(L"GBuffer");
	mDirectionalLight->GetSphere()->GetMaterial()->SetShader(L"GBuffer"); // ���Ǿ� �����ϴ°���.
	mSphere->GetMaterial()->SetShader(L"GBuffer");
	mTerrain->GetMaterial()->SetShader(L"GBuffer");
	// GBuffer�� �ȼ����̴����� OM�� ����Ÿ�ٿ� float4���·� ���Ͻ��ѳ���.

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
	mGBuffer->SetRenderTargetsToPS(); // OM�� ���õǾ��ִ� RTV���� SRV�� �ȼ����̴��� Set. ���۵�����ü��̴����� ����Ұ���.

	// ����ȭ�� ��������.
	mVertexBuffer->IASet(); // ����Ʈ 0��.
	DEVICECONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	mDeferredMaterial->Set(); // ���۵�����ü��̴����� Set. �̰� Set�ϰ� Draw������ �� �´�.
	DEVICECONTEXT->Draw(4, 0);
	mGBuffer->ClearSRVs();
}

void DeferredRenderingScene::PostRender()
{
	mGBuffer->PostRender(); // UIImage�� ����.

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

