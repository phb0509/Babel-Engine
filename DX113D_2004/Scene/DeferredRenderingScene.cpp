#include "Framework.h"
#include "DeferredRenderingScene.h"

DeferredRenderingScene::DeferredRenderingScene()
{
	mWorldCamera = new Camera();
	mWorldCamera->mPosition = { 0, 10, -20 };
	mWorldCamera->mRotation.x = 0.4f;

	// Create Models
	mBunny = new ModelObject();
	mBunny->mScale = { 0.01f, 0.01f, 0.01f };
	mBunny->mPosition.y = 2.35f;
	mBunny->mPosition.x = -3.0f;

	mPlane = new ModelObject();
	mPlane->mScale = { 2.0f, 2.0f, 2.0f };

	mGroot = new ModelAnimObject();
	mGroot->mScale = { 0.01f, 0.01f, 0.01f };
	mGroot->mPosition.x = 3.0f;
	mGroot->SetShader(L"ModelAnimation");
	//groot->ReadClip("Groot/Dancing0");

	mGroot->SetMesh("Groot", "Groot.mesh");
	//mGroot->SetMaterial("Groot", "Groot.mat");
	mGroot->ReadClip("Groot", "Dancing0.clip");

	mGroot->SetDiffuseMap(L"ModelData/Groot/groot_diffuse.png");
	//mGroot->SetSpecularMap(L"ModelData/Groot/groot_glossiness.png");
	mGroot->SetNormalMap(L"ModelData/Groot/groot_normal_map.png");

	int a = 0;

	mSphere = new Sphere(L"Lighting");
	mSphere->mPosition = { 0.0f, 1.0f, -3.0f };
	mSphere->GetMaterial()->SetDiffuseMap(L"Textures/Fieldstone_DM.tga");
	mSphere->GetMaterial()->SetNormalMap(L"Textures/fieldstone_NM.tga");



	mGBuffer = new GBuffer(); // rtv����

	mMaterial = new Material(L"DeferredLighting");

	createMesh();
}

DeferredRenderingScene::~DeferredRenderingScene()
{
	delete mBunny;
	delete mPlane;
	delete mGroot;
	delete mSphere;

	delete mGBuffer;

	delete mMaterial;
	delete mVertexBuffer;
}

void DeferredRenderingScene::Update()
{
	mWorldCamera->Update();
	moveWorldCamera();
	mGroot->Update();
	mSphere->Update();
}

void DeferredRenderingScene::PreRender()
{
	Environment::Get()->Set(); // SetViewPort
	mWorldCamera->SetViewBuffer();
	mWorldCamera->SetProjectionBuffer();
	
	mGBuffer->PreRender(); // ���⼭ OM�� Set. (rtv 3��, dev 1��)

	mBunny->GetModel()->SetShader(L"GBuffer");
	mGroot->SetShader(L"GBuffer");
	mSphere->GetMaterial()->SetShader(L"GBuffer");
	mPlane->GetModel()->SetShader(L"GBuffer");
	// ���۵�����ü��̴����� �� �ؽ��ĵ� 4�� ����.
	// GBuffer�� �ȼ����̴����� OM�� ����Ÿ�ٿ� float4���·� ���Ͻ��ѳ���.

	mBunny->Render(); // �� �������� ���������������� ȣ����.
	mGroot->Render();
	mSphere->Render();
	mPlane->Render();

	// Device::Clear() // ����̽��� RTV,DSV Ŭ���� �ϰ� ������ �Ѿ.
}

void DeferredRenderingScene::Render()
{
	Device::Get()->ClearRenderTargetView(Float4(0.18f, 0.18f, 0.25f, 1.0f));
	Device::Get()->ClearDepthStencilView();
	Device::Get()->SetRenderTarget();
	Environment::Get()->Set(); // SetViewPort

	mWorldCamera->GetViewBuffer()->SetPSBuffer(3);
	mWorldCamera->GetProjectionBufferInUse()->SetPSBuffer(2);

	mGBuffer->Render(); // OM�� ���õǾ��ִ� RTV���� SRV�� �ȼ����̴��� Set. ���۵�����ü��̴����� ����Ұ���.

	// ����ȭ�� ��������.
	mVertexBuffer->IASet(); // ����Ʈ 0��.
	DEVICECONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	mMaterial->Set(); // ���۵�����ü��̴����� Set. �̰� Set�ϰ� Draw������ �� �´�.

	DEVICECONTEXT->Draw(4, 0);
}

void DeferredRenderingScene::PostRender()
{
	mGBuffer->PostRender(); // UIImage�� ����.
}

void DeferredRenderingScene::createMesh()
{
	UINT vertices[4] = { 0, 1, 2, 3 };
	mVertexBuffer = new VertexBuffer(vertices, sizeof(UINT), 4);
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

