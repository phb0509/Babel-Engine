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
	mPreviousMousePosition = MOUSEPOS;

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


	/*Vector3 cameraForward = { 0,0,1 };
	Vector3 gizmoX = { 1,0,0 };

	float dotResult = Vector3::Dot(cameraForward, gizmoX);
	float accosResult = acos(dotResult);

	int a = 0;

	cameraForward = { 0,0,-1 };

	dotResult = Vector3::Dot(cameraForward, gizmoX);
	accosResult = acos(dotResult);

	int b = 0;*/

	Vector3 cameraForward = { 0,0,1 };
	Vector3 gizmoX = { 1,0,0 };
	Vector3 crossResult = Vector3::Cross(cameraForward, gizmoX);

	int a = 0;

	cameraForward = { 0,0,-1 };
	gizmoX = { 1,0,0 };
	crossResult = Vector3::Cross(cameraForward, gizmoX);

	int b = 0;
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

	mTerrain->Update();

	for (Collider* collider : mColliders)
	{
		collider->Update();
	}

	colorPicking();

	if (KEY_DOWN(VK_LBUTTON))
	{
		for (Collider* collider : mColliders)
		{
			Vector3 colliderColor = collider->GetHashColor();

			if (mMousePositionColor.IsEqual(colliderColor)) // �ö��̴��� ��ŷ�ߴٸ�
			{
				mPickedGizmo = 0;
				mPickedCollider = collider;
				mPickedCollider->SetColor(Float4(1.0f, 1.0f, 0.0f, 1.0f)); // ��ŷ�� �ö��̴��� �����
			}
			else // �ö��̴� ���� ���� ��ŷ�ߴٸ�
			{
				if (mPickedCollider != nullptr)
				{
					if (mMousePositionColor.IsEqual(mPickedCollider->GetGizmosHashColorX())) // X�� ����� Ŭ���ߴٸ�
					{
						mPickedGizmo = 1;
						mPickedCollider->SetGizmoXColor(Float4(1.0f, 1.0f, 0.0f, 1.0f));
						mPickedCollider->SetGizmoYColor(Float4(0.5f, 0.5f, 0.5f, 1.0f));
						mPickedCollider->SetGizmoZColor(Float4(0.5f, 0.5f, 0.5f, 1.0f));
					}
					else if (mMousePositionColor.IsEqual(mPickedCollider->GetGizmosHashColorY())) // Y�� ����� Ŭ���ߴٸ�
					{
						mPickedGizmo = 2;
						mPickedCollider->SetGizmoXColor(Float4(0.5f, 0.5f, 0.5f, 1.0f));
						mPickedCollider->SetGizmoYColor(Float4(1.0f, 1.0f, 0.0f, 1.0f));
						mPickedCollider->SetGizmoZColor(Float4(0.5f, 0.5f, 0.5f, 1.0f));
					}
					else if (mMousePositionColor.IsEqual(mPickedCollider->GetGizmosHashColorZ())) // Z�� ����� Ŭ���ߴٸ�
					{
						mPickedGizmo = 3;
						mPickedCollider->SetGizmoXColor(Float4(0.5f, 0.5f, 0.5f, 1.0f));
						mPickedCollider->SetGizmoYColor(Float4(0.5f, 0.5f, 0.5f, 1.0f));
						mPickedCollider->SetGizmoZColor(Float4(1.0f, 1.0f, 0.0f, 1.0f));
					}
					else // ����� Ŭ���ߴٸ�,
					{
						mPickedGizmo = 0;
						collider->SetColor(Float4(0.0f, 1.0f, 0.0f, 1.0f)); // ��ŷ�ȵ����� �ٽ� �ʷϻ�.
						mPickedCollider->SetGizmoXColor(Float4(1.0f, 0.0f, 0.0f, 1.0f));
						mPickedCollider->SetGizmoYColor(Float4(0.0f, 1.0f, 0.0f, 1.0f));
						mPickedCollider->SetGizmoZColor(Float4(0.0f, 0.0f, 1.0f, 1.0f));
						mPickedCollider = nullptr;
					}
				}
			}
		}
	}


	if (mPickedCollider != nullptr)
	{
		// ��ŷ������Ʈ�� x���������
		Vector3 gizmoXVector = { mPickedCollider->Right().x,mPickedCollider->Right().y,mPickedCollider->Right().z };
		Vector3 gizmoYVector = { mPickedCollider->Up().x, mPickedCollider->Up().y,mPickedCollider->Up().z };
		Vector3 gizmoZVector = { mPickedCollider->Forward().x, mPickedCollider->Up().y,mPickedCollider->Up().z };


		// X�� ��������

		// ī�޶������庤�Ϳ� x��������� ����
		Vector3 cameraForward = { WORLDCAMERA->Forward().x,WORLDCAMERA->Forward().y,WORLDCAMERA->Forward().z };
		Vector3 forwardCrossResult = Vector3::Cross(cameraForward, gizmoXVector);

		// ī�޶����Ʈ���Ϳ� x��������� ����
		Vector3 cameraRight = { WORLDCAMERA->Right().x,WORLDCAMERA->Right().y,WORLDCAMERA->Right().z };
		Vector3 rightCrossResult = Vector3::Cross(cameraRight, gizmoXVector);

		
		// Y�� ��������

		
		



		Vector3 mouseValueDifference = MOUSEPOS - mPreviousMousePosition; // ���� �������� ���콺Ŀ������ ���� �������� ���콺Ŀ������ ����.
		float gizmoMoveSpeed = 40.0f;

		if (KEY_PRESS(VK_LBUTTON))
		{
			switch (mPickedGizmo)
			{
			case 0: // ����� Ŭ���� �ȵž��ִ� ���.
				break;
			case 1: // x�� ����� ��ŷ�Ǿ��ִ� ����.
			{
				// ���� ������ ������.
				if (forwardCrossResult.y >= 0)
				{
					if (rightCrossResult.y >= 0) // 4��и�
					{
						if (mouseValueDifference.x >= 0) // ���������� �巡�� �� ���. +�� ���Ѵ�.
						{
							mPickedCollider->mPosition += mPickedCollider->Right() * mouseValueDifference.x * gizmoMoveSpeed * DELTA;
						}
						else if(mouseValueDifference.x < 0)// �������� �巡�� �� ���
						{
							mPickedCollider->mPosition += mPickedCollider->Right() * mouseValueDifference.x * gizmoMoveSpeed * DELTA;
						}

						if (mouseValueDifference.y >= 0) // �Ʒ������� �巡�� �� ���. +�� ���Ѵ�.
						{
							mPickedCollider->mPosition += mPickedCollider->Right() * mouseValueDifference.y * gizmoMoveSpeed * DELTA;
						}
						else if(mouseValueDifference.y < 0) // �������� �巡�� �� ���
						{
							mPickedCollider->mPosition += mPickedCollider->Right() * mouseValueDifference.y * gizmoMoveSpeed * DELTA;
						}
					}

					if (rightCrossResult.y < 0) // 1��и�
					{
						if (mouseValueDifference.x >= 0) // ���������� �巡�� �� ���.+�� ���Ѵ�.
						{
							mPickedCollider->mPosition += mPickedCollider->Right() * mouseValueDifference.x * gizmoMoveSpeed * DELTA;
						}
						else if (mouseValueDifference.x < 0)// �������� �巡�� �� ���. 
						{
							mPickedCollider->mPosition += mPickedCollider->Right() * mouseValueDifference.x * gizmoMoveSpeed * DELTA;
						}

						if (mouseValueDifference.y >= 0) // �Ʒ������� �巡�� �� ���. 
						{
							mPickedCollider->mPosition += mPickedCollider->Right() * mouseValueDifference.y * gizmoMoveSpeed * -1.0f * DELTA;
						}

						else if (mouseValueDifference.y < 0) // �������� �巡�� �� ���. +�� ���Ѵ�.
						{
							mPickedCollider->mPosition += mPickedCollider->Right() * mouseValueDifference.y * gizmoMoveSpeed * -1.0f * DELTA;
						}
					}
				}
				else if (forwardCrossResult.y < 0)
				{
					if (rightCrossResult.y >= 0) // 3��и�
					{
						if (mouseValueDifference.x >= 0) // ���������� �巡�� �� ���. 
						{
							mPickedCollider->mPosition += mPickedCollider->Right() * mouseValueDifference.x * gizmoMoveSpeed * -1.0f * DELTA;
						}
						else if (mouseValueDifference.x < 0)// �������� �巡�� �� ���. +�� ���Ѵ�.
						{
							mPickedCollider->mPosition += mPickedCollider->Right() * mouseValueDifference.x * gizmoMoveSpeed * -1.0f * DELTA;
						}

						if (mouseValueDifference.y >= 0) // �Ʒ������� �巡�� �� ���. +�� ���Ѵ�.
						{
							mPickedCollider->mPosition += mPickedCollider->Right() * mouseValueDifference.y * gizmoMoveSpeed * DELTA;
						}
						else if (mouseValueDifference.y < 0) // �������� �巡�� �� ���
						{
							mPickedCollider->mPosition += mPickedCollider->Right() * mouseValueDifference.y * gizmoMoveSpeed * DELTA;
						}
					}
					if (rightCrossResult.y < 0) // 2��и�
					{
						if (mouseValueDifference.x >= 0) // ���������� �巡�� �� ���. 
						{
							mPickedCollider->mPosition += mPickedCollider->Right() * mouseValueDifference.x * gizmoMoveSpeed * -1.0f * DELTA;
						}
						else if (mouseValueDifference.x < 0)// �������� �巡�� �� ���. +�� ���Ѵ�.
						{
							mPickedCollider->mPosition += mPickedCollider->Right() * mouseValueDifference.x * gizmoMoveSpeed * -1.0f * DELTA;
						}

						if (mouseValueDifference.y >= 0) // �Ʒ������� �巡�� �� ���. 
						{
							mPickedCollider->mPosition += mPickedCollider->Right() * mouseValueDifference.y * gizmoMoveSpeed * -1.0f * DELTA;
						}
						else if (mouseValueDifference.y < 0) // �������� �巡�� �� ���. +�� ���Ѵ�.
						{
							mPickedCollider->mPosition += mPickedCollider->Right() * mouseValueDifference.y * gizmoMoveSpeed * -1.0f * DELTA;
						}
					}
				}
			}

			break;
			case 2: // y�� ����� ��ŷ�Ǿ��ִ� ����.
			{

			}
				break;
			case 3:
				break;
			default:
				break;
			}
		}
	}


	mPreviousMousePosition = MOUSEPOS;

	if (mPickedCollider != nullptr)
	{
		if (KEY_PRESS('W'))
		{
			mPickedCollider->mPosition += mPickedCollider->Forward() * 50.0F * DELTA;
		}
		if (KEY_PRESS('S'))
		{
			mPickedCollider->mPosition += mPickedCollider->Forward() * 50.0F * -1.0f * DELTA;
		}
		if (KEY_PRESS('A'))
		{
			mPickedCollider->mPosition += mPickedCollider->Right() * 50.0F * -1.0f * DELTA;
		}
		if (KEY_PRESS('D'))
		{
			mPickedCollider->mPosition += mPickedCollider->Right() * 50.0F * DELTA;
		}
		if (KEY_PRESS('Q'))
		{
			mPickedCollider->mPosition += mPickedCollider->Up() * 50.0F * DELTA;
		}
		if (KEY_PRESS('E'))
		{
			mPickedCollider->mPosition += mPickedCollider->Up() * 50.0F * -1.0f * DELTA;
		}
		if (KEY_PRESS('F'))
		{
			mPickedCollider->mRotation.y += 10.0f * DELTA;
		}
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

	mTerrain->Render();

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

	//ImGui::InputFloat3("BoxCollider Color", (float*)&temp);
	//SpacingRepeatedly(2);

	//temp = mBoxCollider->GetGizmosHashColorX();
	//ImGui::InputFloat3("Gizmos X HashColor", (float*)&temp);
	//SpacingRepeatedly(2);

	//temp = mBoxCollider->GetGizmosHashColorY();
	//ImGui::InputFloat3("Gizmos Y HashColor Color", (float*)&temp);
	//SpacingRepeatedly(2);

	//temp = mBoxCollider->GetGizmosHashColorZ();
	//ImGui::InputFloat3("Gizmos Z HashColor Color", (float*)&temp);
	//SpacingRepeatedly(3);

	temp = mBoxCollider->Forward();
	ImGui::InputFloat3("BoxCollider Forward", (float*)&temp);
	SpacingRepeatedly(2);

	temp = mBoxCollider->Right();
	ImGui::InputFloat3("BoxCollider Right", (float*)&temp);
	SpacingRepeatedly(2);

	temp = mBoxCollider->Up();
	ImGui::InputFloat3("BoxCollider Up", (float*)&temp);
	SpacingRepeatedly(2);

	temp = WORLDCAMERA->Forward();
	ImGui::InputFloat3("WorldCamera Forward", (float*)&temp);
	SpacingRepeatedly(2);

	ImGui::InputFloat3("MousePosition Color", (float*)&mMousePositionColor);
	SpacingRepeatedly(2);

	ImGui::InputInt2("Mouse Position", (int*)&mousePosition);
	SpacingRepeatedly(2);

	ImGui::End();


	ImGui::Begin("Gizmo X");
	Vector3 cameraForward = { WORLDCAMERA->Forward().x,WORLDCAMERA->Forward().y,WORLDCAMERA->Forward().z };
	Vector3 colliderRight = { mBoxCollider->Right().x,mBoxCollider->Right().y,mBoxCollider->Right().z };
	Vector3 tempForwardCrossResult = Vector3::Cross(cameraForward, colliderRight);
	ImGui::InputFloat3("Forward Corss Result", (float*)&tempForwardCrossResult);
	SpacingRepeatedly(2);

	Vector3 cameraRight = { WORLDCAMERA->Right().x,WORLDCAMERA->Right().y,WORLDCAMERA->Right().z };
	Vector3 tempRightCrossResult = Vector3::Cross(cameraRight, colliderRight);
	ImGui::InputFloat3("Right Corss Result", (float*)&tempRightCrossResult);
	SpacingRepeatedly(2);
	ImGui::End();



	ImGui::Begin("Gizmo Y");
	Vector3 cameraForward = { WORLDCAMERA->Forward().x,WORLDCAMERA->Forward().y,WORLDCAMERA->Forward().z };
	Vector3 colliderUp = { mBoxCollider->Up().x,mBoxCollider->Up().y,mBoxCollider->Up().z };
	Vector3 tempForwardCrossResult = Vector3::Cross(cameraForward, colliderUp);
	ImGui::InputFloat3("Forward Corss Result", (float*)&tempForwardCrossResult);
	SpacingRepeatedly(2);

	Vector3 cameraUp = { WORLDCAMERA->Right().x,WORLDCAMERA->Right().y,WORLDCAMERA->Right().z };
	Vector3 tempRightCrossResult = Vector3::Cross(cameraRight, colliderRight);
	ImGui::InputFloat3("Right Corss Result", (float*)&tempRightCrossResult);
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


