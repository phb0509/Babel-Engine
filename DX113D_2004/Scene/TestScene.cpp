#include "Framework.h"
#include "TestScene.h"



TestScene::TestScene()
{
	Environment::Get()->SetIsEnabledTargetCamera(false); // 월드카메라만 사용.
	mBoxCollider = new BoxCollider();

	mBoxCollider->mScale = { 20.0f,20.0f,20.0f };
	
	WORLDCAMERA->mPosition = { -46.0f, 23.8f, -75};
	WORLDCAMERA->mRotation = { 0.3f,0.5f,0.0f };

	//ImApp::ImApp imApp;

	//ImApp::Config config;
	//config.mWidth = 1280;
	//config.mHeight = 720;
	////config.mFullscreen = true;
	//imApp.Init(config);

}

TestScene::~TestScene()
{
}

void TestScene::Update()
{
	if (Environment::Get()->GetIsEnabledTargetCamera())
	{
		Environment::Get()->GetTargetCamera()->Update();
	}

	Environment::Get()->GetWorldCamera()->Update();

	mBoxCollider->Update();
}

void TestScene::PreRender()
{

}

void TestScene::Render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	Device::Get()->ClearRenderTargetView();
	Device::Get()->ClearDepthStencilView();
	Device::Get()->SetRenderTarget(); // SetMainRenderTarget

	Environment::Get()->SetPerspectiveProjectionBuffer();

	if (Environment::Get()->GetIsEnabledTargetCamera())
	{
		Environment::Get()->GetTargetCamera()->Render();
	}

	Environment::Get()->GetWorldCamera()->Render(); // FrustumRender 외엔 뭐 업승ㅁ.
	Environment::Get()->Set(); // SetViewPort

	mBoxCollider->Render();

	Matrix viewMatrix = WORLDCAMERA->GetViewMatrix();
	Matrix projectionMatrix = Environment::Get()->GetProjectionMatrix();
	Matrix worldMatrix = mBoxCollider->GetWorldMatrixTemp();

	float cameraView[16] =
	{
		viewMatrix.r[0].m128_f32[0], viewMatrix.r[0].m128_f32[1], viewMatrix.r[0].m128_f32[2], viewMatrix.r[0].m128_f32[3],
		viewMatrix.r[1].m128_f32[0], viewMatrix.r[1].m128_f32[1], viewMatrix.r[1].m128_f32[2], viewMatrix.r[1].m128_f32[3],
		viewMatrix.r[2].m128_f32[0], viewMatrix.r[2].m128_f32[1], viewMatrix.r[2].m128_f32[2], viewMatrix.r[2].m128_f32[3],
		viewMatrix.r[3].m128_f32[0], viewMatrix.r[3].m128_f32[1], viewMatrix.r[3].m128_f32[2], viewMatrix.r[3].m128_f32[3]
	};

	float cameraProjection[16] =
	{
		projectionMatrix.r[0].m128_f32[0], projectionMatrix.r[0].m128_f32[1], projectionMatrix.r[0].m128_f32[2], projectionMatrix.r[0].m128_f32[3],
		projectionMatrix.r[1].m128_f32[0], projectionMatrix.r[1].m128_f32[1], projectionMatrix.r[1].m128_f32[2], projectionMatrix.r[1].m128_f32[3],
		projectionMatrix.r[2].m128_f32[0], projectionMatrix.r[2].m128_f32[1], projectionMatrix.r[2].m128_f32[2], projectionMatrix.r[2].m128_f32[3],
		projectionMatrix.r[3].m128_f32[0], projectionMatrix.r[3].m128_f32[1], projectionMatrix.r[3].m128_f32[2], projectionMatrix.r[3].m128_f32[3]
	};

	float objectTransformMatrix[16] =
	{
		worldMatrix.r[0].m128_f32[0], worldMatrix.r[0].m128_f32[1], worldMatrix.r[0].m128_f32[2], worldMatrix.r[0].m128_f32[3],
		worldMatrix.r[1].m128_f32[0], worldMatrix.r[1].m128_f32[1], worldMatrix.r[1].m128_f32[2], worldMatrix.r[1].m128_f32[3],
		worldMatrix.r[2].m128_f32[0], worldMatrix.r[2].m128_f32[1], worldMatrix.r[2].m128_f32[2], worldMatrix.r[2].m128_f32[3],
		worldMatrix.r[3].m128_f32[0], worldMatrix.r[3].m128_f32[1], worldMatrix.r[3].m128_f32[2], worldMatrix.r[3].m128_f32[3]
	};

	



	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();

	//float windowWidth = (float)ImGui::GetWindowWidth();
	//float windowHeight = (float)ImGui::GetWindowHeight();
	float windowWidth = (float)WIN_WIDTH;
	float windowHeight = (float)WIN_HEIGHT;
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

	ImGuizmo::Manipulate( // 실제 기즈모 렌더코드.
		cameraView, // 뷰행렬 (float*)
		cameraProjection, // 투영행렬 (float*)
		ImGuizmo::OPERATION::TRANSLATE, // ImGuizmo::OPERATION::TRANSLATE
		ImGuizmo::LOCAL,
		objectTransformMatrix,
		NULL
	);



	/*DirectWrite::Get()->GetDC()->EndDraw();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());*/
}

void TestScene::PostRender()
{
	Environment::Get()->SetPerspectiveProjectionBuffer();

	//Matrix viewMatrix = WORLDCAMERA->GetViewMatrix();
	//Matrix projectionMatrix = Environment::Get()->GetProjectionMatrix();
	//Matrix worldMatrix = mBoxCollider->GetWorldMatrixTemp();

	//float cameraView[16] =
	//{
	//	viewMatrix.r[0].m128_f32[0], viewMatrix.r[0].m128_f32[1], viewMatrix.r[0].m128_f32[2], viewMatrix.r[0].m128_f32[3],
	//	viewMatrix.r[1].m128_f32[0], viewMatrix.r[1].m128_f32[1], viewMatrix.r[1].m128_f32[2], viewMatrix.r[1].m128_f32[3],
	//	viewMatrix.r[2].m128_f32[0], viewMatrix.r[2].m128_f32[1], viewMatrix.r[2].m128_f32[2], viewMatrix.r[2].m128_f32[3],
	//	viewMatrix.r[3].m128_f32[0], viewMatrix.r[3].m128_f32[1], viewMatrix.r[3].m128_f32[2], viewMatrix.r[3].m128_f32[3]
	//};

	//float cameraProjection[16] =
	//{
	//	projectionMatrix.r[0].m128_f32[0], projectionMatrix.r[0].m128_f32[1], projectionMatrix.r[0].m128_f32[2], projectionMatrix.r[0].m128_f32[3],
	//	projectionMatrix.r[1].m128_f32[0], projectionMatrix.r[1].m128_f32[1], projectionMatrix.r[1].m128_f32[2], projectionMatrix.r[1].m128_f32[3],
	//	projectionMatrix.r[2].m128_f32[0], projectionMatrix.r[2].m128_f32[1], projectionMatrix.r[2].m128_f32[2], projectionMatrix.r[2].m128_f32[3],
	//	projectionMatrix.r[3].m128_f32[0], projectionMatrix.r[3].m128_f32[1], projectionMatrix.r[3].m128_f32[2], projectionMatrix.r[3].m128_f32[3]
	//};

	//float objectTransformMatrix[16] =
	//{
	//	worldMatrix.r[0].m128_f32[0], worldMatrix.r[0].m128_f32[1], worldMatrix.r[0].m128_f32[2], worldMatrix.r[0].m128_f32[3],
	//	worldMatrix.r[1].m128_f32[0], worldMatrix.r[1].m128_f32[1], worldMatrix.r[1].m128_f32[2], worldMatrix.r[1].m128_f32[3],
	//	worldMatrix.r[2].m128_f32[0], worldMatrix.r[2].m128_f32[1], worldMatrix.r[2].m128_f32[2], worldMatrix.r[2].m128_f32[3],
	//	worldMatrix.r[3].m128_f32[0], worldMatrix.r[3].m128_f32[1], worldMatrix.r[3].m128_f32[2], worldMatrix.r[3].m128_f32[3]
	//};

	

	//ImGuizmo::SetOrthographic(false);
	//ImGuizmo::SetDrawlist();

	///*float windowWidth = (float)ImGui::GetWindowWidth();
	//float windowHeight = (float)ImGui::GetWindowHeight();*/
	//float windowWidth = (float)WIN_WIDTH;
	//float windowHeight = (float)WIN_HEIGHT;

	//ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

	//ImGuizmo::Manipulate( // 실제 기즈모 렌더코드.
	//	cameraView, // 뷰행렬 (float*)
	//	cameraProjection, // 투영행렬 (float*)
	//	ImGuizmo::OPERATION::TRANSLATE, // ImGuizmo::OPERATION::TRANSLATE
	//	ImGuizmo::LOCAL,
	//	objectTransformMatrix,
	//	NULL
	//);

	////ImGui::InputFloat3(Position.c_str(), (float*)&mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider->mPosition);
	//SpacingRepeatedly(3);

	ImGui::Begin("test");

	ImGui::InputFloat3("Translation", (float*)&mBoxCollider->mPosition);
	ImGui::InputFloat3("Rotation", (float*)&mBoxCollider->mRotation);
	ImGui::InputFloat3("Scale", (float*)&mBoxCollider->mScale);

	ImGui::End();

}
