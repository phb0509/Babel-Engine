#include "Framework.h"
#include "TestScene.h"

TestScene::TestScene()
{
	mWorldCamera = new Camera();
	mWorldCamera->mTag = "WorldCamera";
	mWorldCamera->mPosition = { 72.8f, 73.5f, -93.0f };
	mWorldCamera->mRotation = { 0.5f, -0.3f, 0.0f };
	mTerrain = new Terrain();
	mTerrain->SetCamera(mWorldCamera);
	mGBuffer = new GBuffer();

	mMaterial = new Material(L"DeferredLighting");

	int row = 10;
	int column = 10;

	mMutantInstanceCount = row * column;

	mInstancingMutants = new InstancingMutants(mMutantInstanceCount, mTerrain); // ModelAnimators
	mInstancingMutants->SetCameraForCulling(mWorldCamera); // Default는 일단 WorldMode.
	mInstancingMutants->SetIsFrustumCullingMode(false);
	mInstanceMutants = mInstancingMutants->GetInstanceObjects(); // InstanceObjectsVector
	vector<Collider*> monsters0Obstacles = {};

	float startX = 100.0f;
	float startZ = 100.0f;
	float gapWidth = 10.0f;
	float gapHeight = 10.0f;

	for (int z = 0; z < column; z++)
	{
		for (int x = 0; x < row; x++)
		{
			int monsterIndex = z * row + x;
			mInstanceMutants[monsterIndex]->SetInstanceIndex(monsterIndex);
			mInstanceMutants[monsterIndex]->mPosition.x = startX + gapWidth * x;
			mInstanceMutants[monsterIndex]->mPosition.z = startZ + gapHeight * z;
			mInstanceMutants[monsterIndex]->SetTerrain(mTerrain, false);
			mInstanceMutants[monsterIndex]->GetAStar()->SetObstacle(monsters0Obstacles);
		}
	}

	mInstancingMutants->SetDiffuseMap(L"ModelData/Mutant/Mutant_diffuse.png");
	mInstancingMutants->SetNormalMap(L"ModelData/Mutant/Mutant_normal.png");

	mRenderTargets = mGBuffer->GetRenderTargets();
	mDepthStencil = mGBuffer->GetDepthStencil();

	// create Mesh.  // 스크리화면으로 통째로렌더할 메쉬.
	UINT vertices[4] = { 0,1,2,3 };
	mVertexBuffer = new VertexBuffer(vertices, sizeof(UINT), 4);
}

TestScene::~TestScene()
{
}

void TestScene::Update()
{
	mWorldCamera->Update();
	moveWorldCamera();

	mTerrain->Update();
	mInstancingMutants->Update();
}

void TestScene::PreRender()
{
	//Device::Get()->ClearRenderTargetView(Float4(0.18f, 0.18f, 0.25f, 1.0f));
	//Device::Get()->ClearDepthStencilView();
	//Device::Get()->SetRenderTarget();
	Environment::Get()->Set(); // SetViewPort
	mWorldCamera->SetViewBuffer(1);
	mWorldCamera->SetProjectionBuffer();

	mGBuffer->PreRender(); // Set RenderTargets

	mInstancingMutants->SetShader(L"GBuffer");
	
	mInstancingMutants->Render();
}

void TestScene::Render()
{
	Device::Get()->ClearRenderTargetView(Float4(0.18f, 0.18f, 0.25f, 1.0f));
	Device::Get()->ClearDepthStencilView();
	Device::Get()->SetRenderTarget();
	Environment::Get()->Set(); // SetViewPort
	mWorldCamera->SetViewBuffer();
	mWorldCamera->SetProjectionBuffer();


	mWorldCamera->GetViewBuffer()->SetPSBuffer(3);
	mWorldCamera->GetProjectionBufferInUse()->SetPSBuffer(2);

	mGBuffer->SetRenderTargetsToPS();

	mTerrain->Render();
	mInstancingMutants->SetShader(L"Models");
	mInstancingMutants->Render();

	mVertexBuffer->IASet();
	DEVICECONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	mMaterial->Set();
	DEVICECONTEXT->Draw(4, 0);
}

void TestScene::PostRender()
{
	mGBuffer->PostRender();

	//ImGui::Begin("RenderTargets");

	//int frame_padding = 0;
	//ImVec2 imageButtonSize = ImVec2(200.0f, 200.0f); // 이미지버튼 크기설정.                     
	//ImVec2 imageButtonUV0 = ImVec2(0.0f, 0.0f); // 출력할이미지 uv좌표설정.
	//ImVec2 imageButtonUV1 = ImVec2(1.0f, 1.0f); // 전체다 출력할거니까 1.
	//ImVec4 imageButtonBackGroundColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // ImGuiWindowBackGroundColor.
	//ImVec4 imageButtonTintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	//ImGui::ImageButton(mDepthStencil->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);
	//ImGui::ImageButton(mRenderTargets[0]->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);
	//ImGui::ImageButton(mRenderTargets[1]->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);
	//ImGui::ImageButton(mRenderTargets[2]->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);
	//
	//ImGui::End();


}

void TestScene::moveWorldCamera()
{
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


	if (KEY_PRESS(VK_RBUTTON))
	{
		Vector3 value = MOUSEPOS - mPreFrameMousePosition;

		mWorldCamera->mRotation.x += value.y * mWorldCamera->mRotationSpeed * DELTA;
		mWorldCamera->mRotation.y += value.x * mWorldCamera->mRotationSpeed * DELTA;
	}

	mPreFrameMousePosition = MOUSEPOS;
	mWorldCamera->SetViewMatrixToBuffer();
}
