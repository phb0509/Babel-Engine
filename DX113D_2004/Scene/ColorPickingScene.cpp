#include "Framework.h"
#include "ColorPickingScene.h"



ColorPickingScene::ColorPickingScene()
{
	srand(static_cast<unsigned int>(time(NULL)));
	Environment::Get()->SetIsEnabledTargetCamera(false); // ����ī�޶� ���.

	// ī�޶� ����.
	WORLDCAMERA->mPosition = { 5.5, 12.6, -15.7 };
	WORLDCAMERA->mRotation = { 0.7, 0.0, 0.0 };
	WORLDCAMERA->mMoveSpeed = 50.0f;

	mTerrain = new Terrain();
	mMonster = new Mutant();
	mCube = new Cube();
	mBoxCollider = new BoxCollider();
	mSphereCollider = new SphereCollider();
	mCapsuleCollider = new CapsuleCollider();
	
	mBoxCollider->mScale = { 5.0f,5.0f,5.0f };
	mSphereCollider->mScale = { 5.0f,5.0f,5.0f };
	mCapsuleCollider->mScale = { 5.0f,5.0f,5.0f };

	mColliders.push_back(mBoxCollider);
	mColliders.push_back(mSphereCollider);
	mColliders.push_back(mCapsuleCollider);

	//Monster->SetTerrain(mTerrain);
	mDepthStencil = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true); // ���̰�

	mRenderTargetTexture = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT);
	mRenderTargets[0] = mRenderTargetTexture;

	mCube->mPosition = { 0.0f,0.0f,0.0f };

	//int n = rand%1000000;

	mBoxCollider->mPosition = { 10.0f,0.0f,0.0f };
	mSphereCollider->mPosition = { 20.0f,0.0f,0.0f };
	mCapsuleCollider->mPosition = { 0.0f,0.0f,0.0f };

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
	for (Collider* collider : mColliders)
	{
		collider->Update();
	}

	colorPicking();

	if (KEY_DOWN(VK_LBUTTON)) // ���콺Ŭ���ÿ��� ����. �ϴ��� ���� �����Ӹ������ʿ� �����ϱ�. 
	{
		for (Collider* collider : mColliders)
		{
			Vector3 colliderColor = collider->GetHashColor();

			if (mMousePositionColor.IsEqual(colliderColor))
			{
				if (KEY_DOWN(VK_LBUTTON))
				{
					mPickedCollider = collider;
					mPickedCollider->SetColor(Float4(1.0f, 1.0f, 0.0f, 1.0f));
				}
			}
			else
			{
				if (KEY_DOWN(VK_LBUTTON))
				{
					collider->SetColor(Float4(0.0f, 1.0f, 0.0f, 1.0f));
				}
			}
		}
	}
}

void ColorPickingScene::PreRender()
{
	RenderTarget::Sets(mRenderTargets, 1, mDepthStencil);

	// �÷���ŷ�� ����Ÿ���ؽ��Ŀ� ����.


	for (Collider* collider : mColliders)
	{
		collider->RenderForColorPicking();
	}

	if (mPickedCollider != nullptr)
	{
		mPickedCollider->RenderGizmosForColorPicking();
	}
}

void ColorPickingScene::Render()
{
	//mTerrain->Render();

	for (Collider* collider : mColliders)
	{
		collider->GetMaterial()->SetShader(L"Collider");
		collider->Render();
	}

	if (mPickedCollider != nullptr)
	{
		mPickedCollider->RenderGizmos();
	}
	
}

void ColorPickingScene::PostRender()
{
	ImGui::Begin("Test Window");

	int frame_padding = 0;
	ImVec2 imageButtonSize = ImVec2(400.0f, 400.0f); // �̹�����ư ũ�⼳��.                     
	ImVec2 imageButtonUV0 = ImVec2(0.0f, 0.0f); // ������̹��� uv��ǥ����.
	ImVec2 imageButtonUV1 = ImVec2(1.0f, 1.0f); // ��ü�� ����ҰŴϱ� 1.
	ImVec4 imageButtonBackGroundColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // ImGuiWindowBackGroundColor.
	ImVec4 imageButtonTintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	// Render to RenderTargetTexture
	ImGui::ImageButton(mRenderTargetTexture->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);

	int32_t mousePositionX = static_cast<int32_t>(MOUSEPOS.x);
	int32_t mousePositionY = static_cast<int32_t>(MOUSEPOS.y);
	Int2 mousePosition = { mousePositionX,mousePositionY };

	ImGui::InputInt2("Mouse Position", (int*)&mousePosition); 
	SpacingRepeatedly(2);

	ImGui::End();
}

void ColorPickingScene::colorPicking()
{
	int32_t mousePositionX = static_cast<int32_t>(MOUSEPOS.x);
	int32_t mousePositionY = static_cast<int32_t>(MOUSEPOS.y);

	Int2 mousePosition = { mousePositionX,mousePositionY };
	mMouseScreenUVPosition = { MOUSEPOS.x / WIN_WIDTH, MOUSEPOS.y / WIN_HEIGHT ,0.0f };
	mInputBuffer->data.mouseScreenUVPosition = { mMouseScreenUVPosition.x,mMouseScreenUVPosition.y }; // ���콺��ǥ uv��
	mInputBuffer->data.mouseScreenPosition = mousePosition; // ���콺��ǥ uv��

	mComputeShader->Set(); // ����̽��� Set..
	mInputBuffer->SetCSBuffer(1);

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mRenderTargetTexture->GetSRV());
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mComputeStructuredBuffer->GetUAV(), nullptr);

	DEVICECONTEXT->Dispatch(1, 1, 1);

	mComputeStructuredBuffer->Copy(mOutputBuffer, sizeof(ColorPickingOutputBuffer)); // GPU���� ����Ѱ� �޾ƿ�. 
	
	mMousePositionColor = mOutputBuffer->color;
}


