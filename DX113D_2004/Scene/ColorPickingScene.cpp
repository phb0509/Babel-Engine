#include "Framework.h"
#include "ColorPickingScene.h"



ColorPickingScene::ColorPickingScene()
{
	Environment::Get()->SetIsEnabledTargetCamera(false); // 월드카메라만 사용.

	// 카메라 설정.
	WORLDCAMERA->mPosition = { 5.5, 12.6, -15.7 };
	WORLDCAMERA->mRotation = { 0.7, 0.0, 0.0 };
	WORLDCAMERA->mMoveSpeed = 50.0f;

	mTerrain = new Terrain();
	mMonster = new Mutant();
	mCube = new Cube();
	mCollider = new BoxCollider();
	
	mCollider->mScale = { 10.0f,10.0f,10.0f };

	mMonster->SetTerrain(mTerrain);

	mDepthStencil = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true); // 깊이값

	mRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT); //
	mRenderTargets[0] = mRenderTarget;

	//mMonster->SetHashColor(5); // 이건 Transform에서 수행. 

	mCube->SetHashColor(15200);

	mCollider->SetHashColor(3245300);
	mCollider->mPosition = { 10.0f,0.0f,0.0f };

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

	int32_t mousePositionX = static_cast<int32_t>(MOUSEPOS.x);
	int32_t mousePositionY = static_cast<int32_t>(MOUSEPOS.y);

	Int2 mousePosition = { mousePositionX,mousePositionY };
	mMouseScreenUVPosition = { MOUSEPOS.x / WIN_WIDTH, MOUSEPOS.y / WIN_HEIGHT ,0.0f };
	mInputBuffer->data.mouseScreenUVPosition = { mMouseScreenUVPosition.x,mMouseScreenUVPosition.y }; // 마우스좌표 uv값
	mInputBuffer->data.mouseScreenPosition = mousePosition; // 마우스좌표 uv값

	mComputeShader->Set(); // 디바이스에 Set..
	mInputBuffer->SetCSBuffer(1);

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mRenderTarget->GetSRV());
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mComputeStructuredBuffer->GetUAV(), nullptr);

	DEVICECONTEXT->Dispatch(1, 1, 1);

	mComputeStructuredBuffer->Copy(mOutputBuffer, sizeof(ColorPickingOutputBuffer)); // GPU에서 계산한거 받아옴. 


	Vector3 colliderColor = mCollider->GetHashColor();
	Vector3 mousePositionColor = mOutputBuffer->color;
	Vector3 cubeColor = mCube->GetHashColor();

	/*if (colliderColor.IsEqual(mousePositionColor))
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
	}*/


	if (mousePositionColor.IsEqual(colliderColor))
	{
		if (KEY_DOWN(VK_LBUTTON))
		{
			//mCube->GetMaterial()->SetDiffuseMap(Texture::Add(L"ModelData/Mesh_PreviewImage.png"));
			mCollider->SetColor(Float4(1.0f, 1.0f, 0.0f, 1.0f));
		}
	}
	else
	{
		if (KEY_DOWN(VK_LBUTTON))
		{
			//mCube->GetMaterial()->SetDiffuseMap(Texture::Add(L"ModelData/DefaultImage.png"));
			mCollider->SetColor(Float4(0.0f, 1.0f, 0.0f, 1.0f));
		}
	}
}

void ColorPickingScene::PreRender()
{
	RenderTarget::Sets(mRenderTargets, 1, mDepthStencil);

	mCube->SetShader(L"ColorPicking"); // 어떤것이든 mMaterial->SetShader(L"wstring");
	mCube->SetWorldBuffer(); // Transform
	mCube->SetColorBuffer(); // Transform
	mCube->SetMesh();

	mCollider->GetMaterial()->SetShader(L"ColorPicking");
	mCollider->SetWorldBuffer();
	mCollider->SetColorBuffer();
	mCollider->SetMeshAndDraw();


}

void ColorPickingScene::Render()
{

	//mTerrain->Render();
	mCube->SetShader(L"Diffuse");
	mCube->Render();

	mCollider->GetMaterial()->SetShader(L"Collider");
	mCollider->Render();



	//mMonster->Render();
}

void ColorPickingScene::PostRender()
{
	ImGui::Begin("Test Window");

	int frame_padding = 0;
	ImVec2 imageButtonSize = ImVec2(150.0f, 150.0f); // 이미지버튼 크기설정.                     
	ImVec2 imageButtonUV0 = ImVec2(0.0f, 0.0f); // 출력할이미지 uv좌표설정.
	ImVec2 imageButtonUV1 = ImVec2(1.0f, 1.0f); // 전체다 출력할거니까 1.
	ImVec4 imageButtonBackGroundColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // ImGuiWindowBackGroundColor.
	ImVec4 imageButtonTintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	ImGui::ImageButton(mRenderTarget->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);

	Float4 cubeHashColor = mCube->GetHashColor();
	Float4 colliderHashColor = mCollider->GetHashColor();
	Float4 returnedColor = mOutputBuffer->color;
	
	int32_t mousePositionX = static_cast<int32_t>(MOUSEPOS.x);
	int32_t mousePositionY = static_cast<int32_t>(MOUSEPOS.y);

	Int2 mousePosition = { mousePositionX,mousePositionY };
	
	ImGui::InputFloat4("Cube HashColorValue", (float*)&cubeHashColor, "%.15f");
	ImGui::InputFloat4("Collider HashColorValue", (float*)&colliderHashColor, "%.15f");
	ImGui::InputFloat4("returned ColorValue", (float*)&returnedColor, "%.15f");

	SpacingRepeatedly(2);

	ImGui::InputInt2("Mouse Position", (int*)&mousePosition);

	ImGui::End();
}


