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
	mPlayer = GM->GetPlayer();
	mPlayer->SetTerrain(mTerrain);


	mMonster = new Mutant();
	mCube = new Cube();
	//mCube->mPosition = { 20.0f,0.0f,0.0f };
	mBoxCollider = new BoxCollider();
	mSphereCollider = new SphereCollider();
	mCapsuleCollider = new CapsuleCollider();

	mColliders.push_back(mBoxCollider);
	mColliders.push_back(mSphereCollider);
	mColliders.push_back(mCapsuleCollider);

	//Monster->SetTerrain(mTerrain);

	mPreRenderTargets[0] = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT); // ���η���.
	mPreRenderTargets[1] = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT); // �÷���ŷ��.
	mPreRenderTargetDSV = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true); // 

	//Device::Get()->InitRenderTargets(mRenderTargets, 1);

	mBoxCollider->mPosition = { 0.0f,20.0f,0.0f };
	mBoxCollider->mScale = { 5.0f,5.0f,5.0f };

	mSphereCollider->mPosition = { 20.0f,20.0f,0.0f };
	mSphereCollider->mScale = { 5.0f,5.0f,5.0f };

	mCapsuleCollider->mPosition = { 40.0f,20.0f,0.0f };
	mCapsuleCollider->mScale = { 5.0f,5.0f,5.0f };


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

	if (KEY_DOWN(VK_LBUTTON))
	{
		for (Collider* collider : mColliders)
		{
			Vector3 colliderColor = collider->GetHashColor();

			if (mMousePositionColor.IsEqual(colliderColor)) // �ö��̴��� ��ŷ�ߴٸ�
			{
				if (mCurrentPickedCollider != nullptr)
				{
					mPreviousPickedCollider = mCurrentPickedCollider;
					mPreviousPickedCollider->SetColor(Float4(0.0f, 1.0f, 0.0f, 1.0f));
				}
			
				mCurrentPickedCollider = collider;
				mCurrentPickedCollider->SetColor(Float4(1.0f, 1.0f, 0.0f, 1.0f)); // ��ŷ�� �ö��̴��� �����
			}
			else
			{
				collider->SetColor(Float4(0.0f, 1.0f, 0.0f, 1.0f));
			}
		}
	}

	if (mCurrentPickedCollider != nullptr)
	{
		if (KEY_PRESS('W'))
		{
			mCurrentPickedCollider->mPosition += mCurrentPickedCollider->Forward() * 50.0F * DELTA;
		}
		if (KEY_PRESS('S'))
		{
			mCurrentPickedCollider->mPosition += mCurrentPickedCollider->Forward() * 50.0F * -1.0f * DELTA;
		}
		if (KEY_PRESS('A'))
		{
			mCurrentPickedCollider->mPosition += mCurrentPickedCollider->Right() * 50.0F * -1.0f * DELTA;
		}
		if (KEY_PRESS('D'))
		{
			mCurrentPickedCollider->mPosition += mCurrentPickedCollider->Right() * 50.0F * DELTA;
		}
		if (KEY_PRESS('Q'))
		{
			mCurrentPickedCollider->mPosition += mCurrentPickedCollider->Up() * 50.0F * DELTA;
		}
		if (KEY_PRESS('E'))
		{
			mCurrentPickedCollider->mPosition += mCurrentPickedCollider->Up() * 50.0F * -1.0f * DELTA;
		}
		if (KEY_PRESS('F'))
		{
			mCurrentPickedCollider->mRotation.y += 10.0f * DELTA;
		}
	}
}

void ColorPickingScene::PreRender()
{
	Environment::Get()->SetPerspectiveProjectionBuffer();
	RenderTarget::ClearAndSetWithDSV(mPreRenderTargets, 2, mPreRenderTargetDSV);
	Environment::Get()->Set(); // ����� Set VS


	if (Environment::Get()->GetIsEnabledTargetCamera())
	{
		Environment::Get()->GetTargetCamera()->Render();
	}

	Environment::Get()->GetWorldCamera()->Render(); // FrustumRender �ܿ� �� ���¤�.

	// �÷���ŷ�� ����Ÿ���ؽ��Ŀ� ����.

	for (Collider* collider : mColliders)
	{
		collider->PreRenderForColorPicking();
		collider->Render();
	}
}

void ColorPickingScene::Render()
{
	// ����ۿ� ����� ����Ÿ��. �������Ұ���.
	Device::Get()->ClearRenderTargetView();
	Device::Get()->ClearDepthStencilView();
	Device::Get()->SetRenderTarget();

	if (Environment::Get()->GetIsEnabledTargetCamera())
	{
		Environment::Get()->GetTargetCamera()->Render();
	}

	Environment::Get()->GetWorldCamera()->Render(); // FrustumRender �ܿ� �� ���¤�.
	Environment::Get()->Set(); // SetViewPort
	Environment::Get()->SetPerspectiveProjectionBuffer();

	for (Collider* collider : mColliders)
	{
		collider->Render();
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
	ImVec4 imageButtonBackGroundColor = ImVec4(0.f, 0.0f, 0.0f, 1.0f); // ImGuiWindowBackGroundColor.
	ImVec4 imageButtonTintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	// Render to RenderTargetTexture
	ImGui::ImageButton(mPreRenderTargets[1]->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);

	int32_t mousePositionX = static_cast<int32_t>(MOUSEPOS.x);
	int32_t mousePositionY = static_cast<int32_t>(MOUSEPOS.y);
	Int2 mousePosition = { mousePositionX,mousePositionY };

	ImGui::InputInt2("Mouse Position", (int*)&mousePosition);
	SpacingRepeatedly(2);

	ImGui::End();


	Matrix viewMatrix;
	Matrix projectionMatrix;
	Matrix* worldMatrix;
	
	

	/*if (mCurrentPickedCollider != nullptr)
	{
		viewMatrix = WORLDCAMERA->GetViewMatrix();
		projectionMatrix = Environment::Get()->GetProjectionMatrix();
		worldMatrix = mCurrentPickedCollider->GetWorldMatrixTemp();
	}
	else
	{
		viewMatrix = XMMatrixIdentity();
		projectionMatrix = XMMatrixIdentity();
		worldMatrix = XMMatrixIdentity();
	}*/
	

	viewMatrix = WORLDCAMERA->GetViewMatrix();
	projectionMatrix = Environment::Get()->GetProjectionMatrix();
	//worldMatrix = mBoxCollider->GetWorldMatrixTemp();
	worldMatrix = mBoxCollider->GetWorldMatrix();
	

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




	float identityMatrix[16] =
	{ 1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f };

	//float windowWidth = (float)ImGui::GetWindowWidth();
	//float windowHeight = (float)ImGui::GetWindowHeight();

	
	int gizmoCount = 1;
	float camDistance = 8.f;
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

	ImGuiIO& io = ImGui::GetIO(); // Ư�� imgui �����찡 �ƴ϶�, ���ι���۷����Ǵ� �����쿡 ���� �ڵ鷯?�����Ű���.
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;
	static ImGuiWindowFlags gizmoWindowFlags = 0;

	ImGuizmo::SetOrthographic(true);
	ImGuizmo::BeginFrame();

	if (ImGui::IsKeyPressed(90)) // z Ű
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69)) // e Ű
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) // r Ű
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	useWindow = false;

	if (useWindow)
	{
		ImGui::SetNextWindowSize(ImVec2(640, 360), ImGuiCond_Appearing);
		ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_Appearing);

		ImGui::Begin("Main", 0, gizmoWindowFlags);

		ImGuizmo::SetDrawlist();

		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight); // ImGuizmo ���� ����. 0�� ����Ÿ���� ������ Image�� ũ�� �Ȱ��� ��Ī����ߵ�.
		viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
		viewManipulateTop = ImGui::GetWindowPos().y;
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		gizmoWindowFlags =
			ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;
	}
	else
	{
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	}
	
	ImGui::Begin("Gizmo Transform");

	//float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(objectTransformMatrix, matrixTranslation, matrixRotation, matrixScale); // ù��° �Ű������� �����ϰ����ϴ� ������Ʈ Ʈ���������.
	ImGui::InputFloat3("Tr", matrixTranslation);
	//ImGui::InputFloat3("Rt", matrixRotation);
	//ImGui::InputFloat3("Sc", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, objectTransformMatrix);

	mBoxCollider->mPosition.x = matrixTranslation[0];
	mBoxCollider->mPosition.y = matrixTranslation[1];
	mBoxCollider->mPosition.z = matrixTranslation[2];


	ImGui::End();

	ImGuizmo::Manipulate( // ���� ����� �����ڵ�.
		cameraView, // ����� (float*)
		cameraProjection, // ������� (float*)
		mCurrentGizmoOperation, // ImGuizmo::OPERATION::TRANSLATE
		ImGuizmo::LOCAL,
		objectTransformMatrix,
		NULL
	);

	if (useWindow)
	{
		ImGui::End();
	}
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

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mPreRenderTargets[1]->GetSRV()); // CS 0�� �������Ϳ� ���ε�.
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mComputeStructuredBuffer->GetUAV(), nullptr);
	DEVICECONTEXT->Dispatch(1, 1, 1);

	ID3D11ShaderResourceView* pSRV = NULL;
	DEVICECONTEXT->CSSetShaderResources(0, 1, &pSRV); // CS 0�� �������Ϳ� NULL�� ����. ���ϸ� ��� ���

	mComputeStructuredBuffer->Copy(mOutputBuffer, sizeof(ColorPickingOutputBuffer)); // GPU���� ����Ѱ� �޾ƿ�. 

	mMousePositionColor = mOutputBuffer->color;
}


