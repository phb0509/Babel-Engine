#include "Framework.h"
#include "ColorPickingScene.h"



ColorPickingScene::ColorPickingScene()
{
	srand(static_cast<unsigned int>(time(NULL)));
	Environment::Get()->SetIsEnabledTargetCamera(false); // ����ī�޶� ���.

	// ī�޶� ����.
	WORLDCAMERA->mPosition = { 0.0f, 0.0f, -40.0f };
	WORLDCAMERA->mRotation = { 0.0, 0.0, 0.0 };
	WORLDCAMERA->mMoveSpeed = 50.0f;

	mTerrain = new Terrain();
	mMonster = new Mutant();
	mCube = new Cube();
	mBoxCollider = new BoxCollider();
	mSphereCollider = new SphereCollider();
	mCapsuleCollider = new CapsuleCollider();
	
	mBoxCollider->mScale = { 2.0f,2.0f,2.0f };
	mSphereCollider->mScale = { 2.0f,2.0f,2.0f };
	mCapsuleCollider->mScale = { 2.0f,2.0f,2.0f };

	mColliders.push_back(mBoxCollider);
	//mColliders.push_back(mSphereCollider);
	//mColliders.push_back(mCapsuleCollider);

	//Monster->SetTerrain(mTerrain);
	mDepthStencil = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true); // ���̰�

	mRenderTargetTexture = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT);
	mRenderTargets[0] = mRenderTargetTexture;

	mBoxCollider->mPosition = { 0.0f,0.0f,0.0f };
	mSphereCollider->mPosition = { 20.0f,0.0f,0.0f };
	mCapsuleCollider->mPosition = { 0.0f,0.0f,0.0f };

	mBoxCollider->mScale = { 10.0f,10.0f,10.0f };


	// Create ComputeShader
	mColorPickingComputeShader = Shader::AddCS(L"ComputeColorPicking");
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
	if (Environment::Get()->GetIsEnabledTargetCamera())
	{
		Environment::Get()->GetTargetCamera()->Update();
	}

	Environment::Get()->GetWorldCamera()->Update();

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
					mPickedCollider = nullptr;
				}
			}
		}
	}


	if (KEY_PRESS('W'))
	{
		mBoxCollider->mPosition.z += 100.0f * DELTA;
	}
	
	if (KEY_PRESS('S'))
	{
		mBoxCollider->mPosition.z -= 100.0f * DELTA;
	}

	if (KEY_PRESS('A'))
	{
		mBoxCollider->mPosition.x -= 100.0f * DELTA;
	}

	if (KEY_PRESS('D'))
	{
		mBoxCollider->mPosition.x += 100.0f * DELTA;
	}
}

void ColorPickingScene::PreRender()
{
	Environment::Get()->Set(); // ����� Set VS
	Environment::Get()->SetPerspectiveProjectionBuffer();
	//RenderTarget::SetWithDSV(mRenderTargets, 1, nullptr);
	RenderTarget::SetWithoutDSV(mRenderTargets, 1);
	//RenderTarget::Sets(mRenderTargets, 1, nullptr);

	// �÷���ŷ�� ����Ÿ���ؽ��Ŀ� ����.

	for (Collider* collider : mColliders)
	{
		collider->PreRenderForColorPicking();
	}

	if (mPickedCollider != nullptr)
	{
		mPickedCollider->Transform::PreRenderGizmosForColorPicking();
	}
}

void ColorPickingScene::Render()
{
	// ���η���Ÿ�� Clear �Ŀ� Set.
	Device::Get()->ClearRenderTargetView();
	Device::Get()->ClearDepthStencilView();
	//Device::Get()->SetRenderTarget(); // SetMainRenderTarget
	Device::Get()->SetRenderTargetNullDSV();

	if (Environment::Get()->GetIsEnabledTargetCamera())
	{
		Environment::Get()->GetTargetCamera()->Render();
	}

	Environment::Get()->GetWorldCamera()->Render(); // FrustumRender �ܿ� �� ���¤�.
	Environment::Get()->Set(); // SetViewPort
	Environment::Get()->SetPerspectiveProjectionBuffer();

	for (Collider* collider : mColliders)
	{
		//Environment::Get()->SetOrthographicProjectionBuffer();
		collider->Render();
	}

	if (mPickedCollider != nullptr)
	{
		mPickedCollider->RenderGizmos();
	}
}

void ColorPickingScene::PostRender()
{
	Environment::Get()->SetPerspectiveProjectionBuffer();

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

	Vector3 temp = mBoxCollider->GetHashColor();

	ImGui::InputFloat3("BoxCollider Color", (float*)&temp);
	SpacingRepeatedly(2);

	temp = mBoxCollider->GetGizmosHashColorX();
	ImGui::InputFloat3("Gizmos X HashColor", (float*)&temp);
	SpacingRepeatedly(2);

	temp = mBoxCollider->GetGizmosHashColorY();
	ImGui::InputFloat3("Gizmos Y HashColor Color", (float*)&temp);
	SpacingRepeatedly(2);

	temp = mBoxCollider->GetGizmosHashColorZ();
	ImGui::InputFloat3("Gizmos Z HashColor Color", (float*)&temp);
	SpacingRepeatedly(3);

	/*temp = mBoxCollider->mPosition;
	ImGui::InputFloat3("BoxCollider Position", (float*)&temp);
	SpacingRepeatedly(2);

	temp = mBoxCollider->mRotation;
	ImGui::InputFloat3("BoxCollider Rotation", (float*)&temp);
	SpacingRepeatedly(2);

	temp = mGizmos->mPosition;
	ImGui::InputFloat3("mGizmos Position", (float*)&temp);
	SpacingRepeatedly(2);

	temp = mGizmos->mRotation;
	ImGui::InputFloat3("mGizmos Rotation", (float*)&temp);
	SpacingRepeatedly(2);*/

	ImGui::InputFloat3("MousePosition Color", (float*)&mMousePositionColor);
	SpacingRepeatedly(2);

	//ImGui::InputInt2("Mouse Position", (int*)&mousePosition);
	//SpacingRepeatedly(2);

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

	mColorPickingComputeShader->Set(); // ����̽��� Set..
	mInputBuffer->SetCSBuffer(1); // CS 1�� �������Ϳ� Set.

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mRenderTargetTexture->GetSRV()); // CS 0�� �������Ϳ� ���ε�.
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mComputeStructuredBuffer->GetUAV(), nullptr);
	DEVICECONTEXT->Dispatch(1, 1, 1);

	ID3D11ShaderResourceView* pSRV = NULL;
	DEVICECONTEXT->CSSetShaderResources(0, 1, &pSRV); // CS 0�� �������Ϳ� NULL�� ����. ���ϸ� ��� ���

	mComputeStructuredBuffer->Copy(mOutputBuffer, sizeof(ColorPickingOutputBuffer)); // GPU���� ����Ѱ� �޾ƿ�. 

	mMousePositionColor = mOutputBuffer->color;
}


