#include "Framework.h"
#include "ColorPickingScene.h"



ColorPickingScene::ColorPickingScene()
{
	Environment::Get()->SetIsEnabledTargetCamera(false); // ����ī�޶� ���.

	// ī�޶� ����.
	WORLDCAMERA->mPosition = { -4.8, 7.6, -4.0 };
	WORLDCAMERA->mRotation = { 0.8, 0.7, 0.0 };
	WORLDCAMERA->mMoveSpeed = 50.0f;

	mTerrain = new Terrain();
	mMonster = new Mutant();

	mMonster->SetTerrain(mTerrain);

	
	mDepthStencil = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true); // ���̰�
	mRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	mRenderTargets[0] = mRenderTarget;

	mMonster->SetHashColor(1); // �̰� Transform���� ����.


	// Create ComputeShader
	mComputeShader = Shader::AddCS(L"ComputeColorPicking");
	mComputeStructuredBuffer = new ComputeStructuredBuffer(sizeof(ColorPickingOutputBuffer), 1);

	if (mInputBuffer == nullptr)
		mInputBuffer = new ColorPickingInputBuffer();

	mOutputBuffer = new ColorPickingOutputBuffer[1];




}

ColorPickingScene::~ColorPickingScene()
{
}

void ColorPickingScene::Update()
{
	//collider->Update();
	mTerrain->Update();
	mMonster->Update();

	mMouseScreenPosition = { MOUSEPOS.x / WIN_WIDTH, MOUSEPOS.y / WIN_HEIGHT ,0.0f };
	mInputBuffer->data.mouseScreenPosition = { mMouseScreenPosition.x,mMouseScreenPosition.y }; // ���콺��ǥ uv��

	mComputeShader->Set(); // ����̽��� Set..
	mInputBuffer->SetCSBuffer(0);

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mRenderTarget->GetSRV());
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mComputeStructuredBuffer->GetUAV(), nullptr);

	DEVICECONTEXT->Dispatch(1, 1, 1);

	mComputeStructuredBuffer->Copy(mOutputBuffer, sizeof(ColorPickingOutputBuffer)); // GPU���� ����Ѱ� �޾ƿ�. 

	DEVICECONTEXT->ClearState();
	//Float4 color = mOutputBuffer->data.color;
}

void ColorPickingScene::PreRender()
{
	mMonster->SetShader(L"ColorPicking");
	mMonster->SetWorldBuffer();
	mMonster->SetColorBuffer();

	RenderTarget::Sets(mRenderTargets, 1, mDepthStencil); 

	mMonster->MeshRender();
}

void ColorPickingScene::Render()
{
	//collider->Render();
	mTerrain->Render();
	mMonster->Render();
}

void ColorPickingScene::PostRender()
{
	ImGui::Begin("Test Window");

	/*ImGui::InputFloat3("Position", (float*)&collider->mPosition);
	ImGui::InputFloat3("Rotation", (float*)&collider->mRotation);
	ImGui::InputFloat3("Scale", (float*)&collider->mScale);*/

	int frame_padding = 0;
	ImVec2 imageButtonSize = ImVec2(100.0f, 100.0f); // �̹�����ư ũ�⼳��.                     
	ImVec2 imageButtonUV0 = ImVec2(0.0f, 0.0f); // ������̹��� uv��ǥ����.
	ImVec2 imageButtonUV1 = ImVec2(1.0f, 1.0f); // ��ü�� ����ҰŴϱ� 1.
	ImVec4 imageButtonBackGroundColor = ImVec4(0.06f, 0.06f, 0.06f, 0.94f); // ImGuiWindowBackGroundColor.
	ImVec4 imageButtonTintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);


	ImGui::ImageButton(mRenderTarget->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);
	//ImGui::ImageButton(texture->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);

	ImGui::InputFloat2("MouseScreenPosition", (float*)&mInputBuffer->data.mouseScreenPosition);
	ImGui::InputFloat4("returned Color", (float*)&mOutputBuffer->data.color);


	char buff[100];
	sprintf_s(buff, "color.r : %f color.g : %f color.b : %f\n", mOutputBuffer->data.color.x, mOutputBuffer->data.color.y, mOutputBuffer->data.color.z);
	OutputDebugStringA(buff);




	ImGui::End();
}


