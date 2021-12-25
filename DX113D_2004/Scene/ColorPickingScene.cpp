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
	mCollider = new BoxCollider();
	mCube->mPosition = { 20.0f,20.0f,0.0f };
	//mCollider->mScale = { 10.0f,10.0f,10.0f };

	mMonster->SetTerrain(mTerrain);

	mDepthStencil = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true); // ���̰�

	mRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT); //
	mRenderTargets[0] = mRenderTarget;

	//mMonster->SetHashColor(5); // �̰� Transform���� ����. 
	mCube->SetHashColor(30000);
	mCollider->SetHashColor(30000);

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
	//mTerrain->Update();
	mCube->Update();
	mCollider->Update();


	mMouseScreenPosition = { MOUSEPOS.x / WIN_WIDTH, MOUSEPOS.y / WIN_HEIGHT ,0.0f };
	mInputBuffer->data.mouseScreenPosition = { mMouseScreenPosition.x,mMouseScreenPosition.y }; // ���콺��ǥ uv��

	mComputeShader->Set(); // ����̽��� Set..
	mInputBuffer->SetCSBuffer(1);

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mRenderTarget->GetSRV());
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mComputeStructuredBuffer->GetUAV(), nullptr);

	DEVICECONTEXT->Dispatch(1, 1, 1);

	mComputeStructuredBuffer->Copy(mOutputBuffer, sizeof(ColorPickingOutputBuffer)); // GPU���� ����Ѱ� �޾ƿ�. 


	Vector3 colliderColor = mCollider->GetHashColor();
	Vector3 mousePositionColor = mOutputBuffer->color;

	if (colliderColor.IsEqual(mousePositionColor))
	{
		if (KEY_DOWN(VK_LBUTTON))
		{
			mCube->GetMaterial()->SetDiffuseMap(Texture::Add(L"ModelData/Mesh_PreviewImage.png"));
			mCollider->SetColor(Float4(1.0f, 1.0f, 0.0f, 1.0f));
		}
	}
	else
	{
		if (KEY_DOWN(VK_LBUTTON))
		{
			mCube->GetMaterial()->SetDiffuseMap(Texture::Add(L"ModelData/DefaultImage.png"));
			mCollider->SetColor(Float4(0.0f, 1.0f, 0.0f, 1.0f));
		}
	}

}

void ColorPickingScene::PreRender()
{
	mCube->SetShader(L"ColorPicking"); // ����̵� mMaterial->SetShader(L"wstring");
	mCube->SetWorldBuffer(); // Transform
	mCube->SetColorBuffer(); // Transform

	mCollider->GetMaterial()->SetShader(L"ColorPicking");
	mCollider->SetWorldBuffer();
	mCollider->SetColorBuffer();

	RenderTarget::Sets(mRenderTargets, 1, mDepthStencil);

	mCube->SetMesh();
	mCollider->SetMeshAndDraw();
	
}

void ColorPickingScene::Render()
{

	//mTerrain->Render();
	mCollider->GetMaterial()->SetShader(L"Diffuse");
	mCollider->Render();

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

	Float4 colliderHashColor = mCollider->GetHashColor();
	Float4 cubeHashColor = mCollider->GetHashColor();
	Float4 returnedColor = mOutputBuffer->color;
	
	ImGui::InputFloat4("Collider HashColorValue", (float*)&colliderHashColor, "%.3f");
	ImGui::InputFloat4("Cube HashColorValue", (float*)&cubeHashColor,"%.3f");
	ImGui::InputFloat4("returned ColorValue", (float*)&returnedColor,"%.3f");

	ImGui::End();
}


