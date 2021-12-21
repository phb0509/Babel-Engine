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
	mCube = new Cube();

	mMonster->SetTerrain(mTerrain);

	mDepthStencil = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true); // ���̰�

	mRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT); //
	mRenderTargets[0] = mRenderTarget;

	//mMonster->SetHashColor(5); // �̰� Transform���� ����. 
	mCube->SetHashColor(30000);

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
	//mMonster->Update();
	mCube->Update();

	mMouseScreenPosition = { MOUSEPOS.x / WIN_WIDTH, MOUSEPOS.y / WIN_HEIGHT ,0.0f };
	mInputBuffer->data.mouseScreenPosition = { mMouseScreenPosition.x,mMouseScreenPosition.y }; // ���콺��ǥ uv��

	mComputeShader->Set(); // ����̽��� Set..
	mInputBuffer->SetCSBuffer(1);

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mRenderTarget->GetSRV());
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mComputeStructuredBuffer->GetUAV(), nullptr);

	DEVICECONTEXT->Dispatch(1, 1, 1);

	mComputeStructuredBuffer->Copy(mOutputBuffer, sizeof(ColorPickingOutputBuffer)); // GPU���� ����Ѱ� �޾ƿ�. 


	Vector3 cubeColor = mCube->GetHashColor();
	Vector3 mousePositionColor = mOutputBuffer->color;

	if (cubeColor.IsEqual(mousePositionColor))
	{
		if (KEY_DOWN(VK_LBUTTON))
		{
			mCube->GetMaterial()->SetDiffuseMap(Texture::Add(L"ModelData/Mesh_PreviewImage.png"));
		}
	}
	else
	{
		if (KEY_DOWN(VK_LBUTTON))
		{
			mCube->GetMaterial()->SetDiffuseMap(Texture::Add(L"ModelData/DefaultImage.png"));
		}
	}

}

void ColorPickingScene::PreRender()
{
	//mMonster->SetShader(L"ColorPicking");
	//mMonster->SetWorldBuffer();
	//mMonster->SetColorBuffer();

	mCube->SetShader(L"ColorPicking");
	mCube->SetWorldBuffer();
	mCube->SetColorBuffer();

	RenderTarget::Sets(mRenderTargets, 1, mDepthStencil);

	//mMonster->MeshRender();
	mCube->SetMesh();
}

void ColorPickingScene::Render()
{
	//collider->Render();
	mTerrain->Render();
	mCube->SetShader(L"Diffuse");
	mCube->Render();
	//mMonster->Render();
}

void ColorPickingScene::PostRender()
{
	ImGui::Begin("Test Window");

	int frame_padding = 0;
	ImVec2 imageButtonSize = ImVec2(150.0f, 150.0f); // �̹�����ư ũ�⼳��.                     
	ImVec2 imageButtonUV0 = ImVec2(0.0f, 0.0f); // ������̹��� uv��ǥ����.
	ImVec2 imageButtonUV1 = ImVec2(1.0f, 1.0f); // ��ü�� ����ҰŴϱ� 1.
	ImVec4 imageButtonBackGroundColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // ImGuiWindowBackGroundColor.
	ImVec4 imageButtonTintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	ImGui::ImageButton(mRenderTarget->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);

	Float4 cpuHashColor = mCube->GetHashColor();
	Float4 returnedColor = mOutputBuffer->color;
	
	ImGui::InputFloat4("CPU HashColorValue", (float*)&cpuHashColor,"%.20f");
	ImGui::InputFloat4("returned ColorValue", (float*)&returnedColor,"%.20f");


	ImGui::End();
}


