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

	mPreRenderTargets[0] = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT); // �÷���ŷ��.
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

	mCube->Update();
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
				changePickedObject();
				mCurrentPickedCollider->SetColor(Float4(1.0f, 1.0f, 0.0f, 1.0f)); // ��ŷ�� �ö��̴��� �����
			}
			else
			{
				collider->SetColor(Float4(0.0f, 1.0f, 0.0f, 1.0f));
			}
		}
	}

	float rotationSpeed = 5.0f;

	if (KEY_PRESS('Q'))
	{
		mCube->mRotation.x += rotationSpeed * DELTA;
	}

	if (KEY_PRESS('A'))
	{
		mCube->mRotation.x -= rotationSpeed * DELTA;
	}

	if (KEY_PRESS('W'))
	{
		mCube->mRotation.y += rotationSpeed * DELTA;
	}

	if (KEY_PRESS('S'))
	{
		mCube->mRotation.y -= rotationSpeed * DELTA;
	}

	if (KEY_PRESS('E'))
	{
		mCube->mRotation.z += rotationSpeed * DELTA;
	}

	if (KEY_PRESS('D'))
	{
		mCube->mRotation.z -= rotationSpeed * DELTA;
	}



	/*if (mCurrentPickedCollider != nullptr)
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
	}*/
}

void ColorPickingScene::PreRender()
{
	Environment::Get()->SetPerspectiveProjectionBuffer();
	RenderTarget::ClearAndSetWithDSV(mPreRenderTargets, 1, mPreRenderTargetDSV);
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

	mCube->Render();
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
	ImGui::ImageButton(mPreRenderTargets[0]->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);

	int32_t mousePositionX = static_cast<int32_t>(MOUSEPOS.x);
	int32_t mousePositionY = static_cast<int32_t>(MOUSEPOS.y);
	Int2 mousePosition = { mousePositionX,mousePositionY };

	ImGui::InputInt2("Mouse Position", (int*)&mousePosition);
	SpacingRepeatedly(2);

	ImGui::End();

	Matrix viewMatrix;
	Matrix projectionMatrix;

	viewMatrix = WORLDCAMERA->GetViewMatrix();
	projectionMatrix = Environment::Get()->GetProjectionMatrix();

	Float4x4 cameraView;
	Float4x4 cameraProjection;

	XMStoreFloat4x4(&cameraView, viewMatrix);
	XMStoreFloat4x4(&cameraProjection, projectionMatrix);


	int gizmoCount = 1;
	float camDistance = 8.f;
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

	ImGuiIO& io = ImGui::GetIO(); // Ư�� imgui �����찡 �ƴ϶�, ���ι���۷����Ǵ� �����쿡 ���� �ڵ鷯?�����Ű���.
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;
	static ImGuiWindowFlags gizmoWindowFlags = 0;
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);

	ImGuizmo::SetOrthographic(true);
	ImGuizmo::BeginFrame();

	if (ImGui::IsKeyPressed(90)) // z Ű
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(88)) // x Ű
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(67)) // c Ű
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::IsKeyPressed(86)) // v Ű
		mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;

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

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}

	if (mCube != nullptr)
	{
		Float4x4 transform;
		float translation[3] = { mCube->mPosition.x, mCube->mPosition.y, mCube->mPosition.z };

		float rotation[3] = { XMConvertToDegrees(mCube->mRotation.x), XMConvertToDegrees(mCube->mRotation.y), XMConvertToDegrees(mCube->mRotation.z) };

		float scale[3] = { mCube->mScale.x, mCube->mScale.y, mCube->mScale.z };
		ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, *transform.m);

		ImGuizmo::Manipulate( // ���� ����� �����ڵ�.
			*cameraView.m, // ����� (float*)
			*cameraProjection.m, // ������� (float*)
			mCurrentGizmoOperation, // ImGuizmo::OPERATION::TRANSLATE
			mCurrentGizmoMode,
			*transform.m, // ���� �����巡�׸����� �� ��Ʈ������ ���ΰ��� ����.
			NULL
		);

		// � ����� Hover�ߴ��� üũ.
		{
			if (ImGuizmo::IsOverTranslationX())
			{
				ImGui::Text("Over TranslationX");
				currentGizmo = ImGuizmo::TRANSLATE_X;
			}
			if (ImGuizmo::IsOverTranslationY())
			{
				ImGui::Text("Over TranslationY");
				currentGizmo = ImGuizmo::TRANSLATE_Y;
			}
			if (ImGuizmo::IsOverTranslationZ())
			{
				ImGui::Text("Over TranslationZ");
				currentGizmo = ImGuizmo::TRANSLATE_Z;
			}

			if (ImGuizmo::IsOverRotationX())
			{
				ImGui::Text("Over RotationX");
				currentGizmo = ImGuizmo::ROTATE_X;
			}
			if (ImGuizmo::IsOverRotationY())
			{
				ImGui::Text("Over RotationY");
				currentGizmo = ImGuizmo::ROTATE_Y;
			}
			if (ImGuizmo::IsOverRotationZ())
			{
				ImGui::Text("Over RotationZ");
				currentGizmo = ImGuizmo::ROTATE_Z;
			}

			if (ImGuizmo::IsOverScaleX())
			{
				ImGui::Text("Over ScaleX");
				currentGizmo = ImGuizmo::SCALE_X;
			}
			if (ImGuizmo::IsOverScaleY())
			{
				ImGui::Text("Over ScaleY");
				currentGizmo = ImGuizmo::SCALE_Y;
			}
			if (ImGuizmo::IsOverScaleZ())
			{
				ImGui::Text("Over ScaleZ");
				currentGizmo = ImGuizmo::SCALE_Z;
			}
		}

		switch (currentGizmo)
		{
		case ImGuizmo::TRANSLATE_X:
		{
			ImGui::Text("currentGizmo is TRANSLATE_X");
		}
		break;

		case ImGuizmo::TRANSLATE_Y:
		{
			ImGui::Text("currentGizmo is TRANSLATE_Y");
		}
		break;

		case ImGuizmo::TRANSLATE_Z:
		{
			ImGui::Text("currentGizmo is TRANSLATE_Z");
		}
		break;

		case ImGuizmo::ROTATE_X:
		{
			ImGui::Text("currentGizmo is ROTATE_X");
		}
		break;

		case ImGuizmo::ROTATE_Y:
		{
			ImGui::Text("currentGizmo is ROTATE_Y");
		}
		break;

		case ImGuizmo::ROTATE_Z:
		{
			ImGui::Text("currentGizmo is ROTATE_Z");
		}
		break;

		case ImGuizmo::SCALE_X:
		{
			ImGui::Text("currentGizmo is SCALE_X");
		}
		break;

		case ImGuizmo::SCALE_Y:
		{
			ImGui::Text("currentGizmo is SCALE_Y");
		}
		break;

		case ImGuizmo::SCALE_Z:
		{
			ImGui::Text("currentGizmo is SCALE_Z");
		}
		break;

		default:
		{
			ImGui::Text("Not Picked");
		}
		break;
		}

		/*if (ImGuizmo::IsUsing())
		{
			if ((currentGizmo == ImGuizmo::TRANSLATE_X) ||
				(currentGizmo == ImGuizmo::TRANSLATE_Y) ||
				(currentGizmo == ImGuizmo::TRANSLATE_Z) ||
				(currentGizmo == ImGuizmo::SCALE_X) ||
				(currentGizmo == ImGuizmo::SCALE_Y) ||
				(currentGizmo == ImGuizmo::SCALE_Z))
			{
				float tempTranslation[3] = { 0.0f,0.0f,0.0f };
				float tempRotation[3] = { 0.0f,0.0f,0.0f };
				float tempScale[3] = { 0.0f,0.0f,0.0f };

				ImGuizmo::DecomposeMatrixToComponents(*transform.m, tempTranslation, tempRotation, tempScale);

				mCube->mPosition = Vector3(tempTranslation[0], tempTranslation[1], tempTranslation[2]);
				mCube->mScale = Vector3(tempScale[0], tempScale[1], tempScale[2]);

				matrixTranslation = mCube->mPosition;
				matrixScale = mCube->mScale;

				ImGui::Text("TRANSLATE and SCALE");
			}

			else if (currentGizmo == ImGuizmo::ROTATE_X)
			{
				float tempTranslation[3] = { 0.0f,0.0f,0.0f };
				float tempRotation[3] = { 0.0f,0.0f,0.0f };
				float tempScale[3] = { 0.0f,0.0f,0.0f };

				ImGuizmo::DecomposeMatrixToComponents(*transform.m, tempTranslation, tempRotation, tempScale);

				mCube->mRotation = Vector3(XMConvertToRadians(tempRotation[0]), XMConvertToRadians(tempRotation[1]), XMConvertToRadians(tempRotation[2]));

				matrixRotation = mCube->mRotation;

				ImGui::Text("This is ROTATE_X");
			}

			else if (currentGizmo == ImGuizmo::ROTATE_Y)
			{
				float tempTranslation[3] = { 0.0f,0.0f,0.0f };
				float tempRotation[3] = { 0.0f,0.0f,0.0f };
				float tempScale[3] = { 0.0f,0.0f,0.0f };

				ImGuizmo::DecomposeMatrixToComponents(*transform.m, tempTranslation, tempRotation, tempScale);

				mCube->mRotation.y = XMConvertToRadians(tempRotation[1]);

				matrixRotation.y = mCube->mRotation.y;
				ImGui::Text("This is ROTATE_Y");
			}

			else if (currentGizmo == ImGuizmo::ROTATE_Z)
			{
				float tempTranslation[3] = { 0.0f,0.0f,0.0f };
				float tempRotation[3] = { 0.0f,0.0f,0.0f };
				float tempScale[3] = { 0.0f,0.0f,0.0f };

				ImGuizmo::DecomposeMatrixToComponents(*transform.m, tempTranslation, tempRotation, tempScale);

				mCube->mRotation = Vector3(XMConvertToRadians(tempRotation[0]), XMConvertToRadians(tempRotation[1]), XMConvertToRadians(tempRotation[2]));

				matrixRotation = mCube->mRotation;
				ImGui::Text("This is ROTATE_Z");
			}
		}*/
		if (ImGuizmo::IsUsing())
		{
			float tempTranslation[3] = { 0.0f,0.0f,0.0f };
			float tempRotation[3] = { 0.0f,0.0f,0.0f };
			float tempScale[3] = { 0.0f,0.0f,0.0f };

			ImGuizmo::DecomposeMatrixToComponents(*transform.m, tempTranslation, tempRotation, tempScale);

			mCube->mPosition = Vector3(tempTranslation[0], tempTranslation[1], tempTranslation[2]);
			mCube->mRotation = Vector3(XMConvertToRadians(tempRotation[0]), XMConvertToRadians(tempRotation[1]), XMConvertToRadians(tempRotation[2]));
			mCube->mScale = Vector3(tempScale[0], tempScale[1], tempScale[2]);

			matrixTranslation = mCube->mPosition;
			matrixRotation = mCube->mRotation;
			matrixScale = mCube->mScale;
		}
	}

	if (useWindow)
	{
		ImGui::End();
	}

	ImGui::InputFloat3("cubePosition", (float*)&mCube->mPosition, "%.5f");
	ImGui::InputFloat3("cubeRotation", (float*)&mCube->mRotation, "%.5f");
	ImGui::InputFloat3("cubeScale", (float*)&mCube->mScale, "%.5f");

	SpacingRepeatedly(3);

	ImGui::InputFloat3("matrixPosition", (float*)&matrixTranslation, "%.5f");
	ImGui::InputFloat3("matrixRotation", (float*)&matrixRotation, "%.5f");
	ImGui::InputFloat3("matrixScale", (float*)&matrixScale, "%.5f");
}

void ColorPickingScene::changePickedObject()
{
	//Matrix tempMatrix = *(mCurrentPickedCollider->GetWorldMatrix());
	Matrix tempMatrix = mCurrentPickedCollider->GetWorldMatrixValue();

	//tempMatrix.m

	MatrixToFloatArray(tempMatrix, objectTransformMatrix);

	/*objectTransformMatrix[0] = tempMatrix.r[0].m128_f32[0];
	objectTransformMatrix[1] = tempMatrix.r[0].m128_f32[1];
	objectTransformMatrix[2] = tempMatrix.r[0].m128_f32[2];
	objectTransformMatrix[3] = tempMatrix.r[0].m128_f32[3];
	objectTransformMatrix[4] = tempMatrix.r[1].m128_f32[0];
	objectTransformMatrix[5] = tempMatrix.r[1].m128_f32[1];
	objectTransformMatrix[6] = tempMatrix.r[1].m128_f32[2];
	objectTransformMatrix[7] = tempMatrix.r[1].m128_f32[3];
	objectTransformMatrix[8] = tempMatrix.r[2].m128_f32[0];
	objectTransformMatrix[9] = tempMatrix.r[2].m128_f32[1];
	objectTransformMatrix[10] = tempMatrix.r[2].m128_f32[2];
	objectTransformMatrix[11] = tempMatrix.r[2].m128_f32[3];
	objectTransformMatrix[12] = tempMatrix.r[3].m128_f32[0];
	objectTransformMatrix[13] = tempMatrix.r[3].m128_f32[1];
	objectTransformMatrix[14] = tempMatrix.r[3].m128_f32[2];
	objectTransformMatrix[15] = tempMatrix.r[3].m128_f32[3];*/
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

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mPreRenderTargets[0]->GetSRV()); // CS 0�� �������Ϳ� ���ε�.
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mComputeStructuredBuffer->GetUAV(), nullptr);
	DEVICECONTEXT->Dispatch(1, 1, 1);

	ID3D11ShaderResourceView* pSRV = NULL;
	DEVICECONTEXT->CSSetShaderResources(0, 1, &pSRV); // CS 0�� �������Ϳ� NULL�� ����. ���ϸ� ��� ���

	mComputeStructuredBuffer->Copy(mOutputBuffer, sizeof(ColorPickingOutputBuffer)); // GPU���� ����Ѱ� �޾ƿ�. 

	mMousePositionColor = mOutputBuffer->color;
}




