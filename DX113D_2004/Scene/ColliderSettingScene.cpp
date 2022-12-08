#include "Framework.h"
#include "ColliderSettingScene.h"


ColliderSettingScene::ColliderSettingScene() :
	mCurrentModel(nullptr),
	mCurrentModelIndex(0),
	mBeforeModelIndex(0),
	mbIsDropEvent(false),
	mbIsWireFrame(true),
	mDraggedFileName(""),
	mDroppedFileName(""),
	mbIsDropped(true),
	mCurrentClipSpeed(1.0f),
	mCurrentClipTakeTime(0.2f),
	mPreFrameMousePosition(MOUSEPOS)
{
	// ���ϵ�� �ݹ��Լ� ����.
	GM->Get()->SetWindowDropEvent(bind(&ColliderSettingScene::playAssetWindowDropEvent, this));

	igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", 0, ".");
	mProjectPath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath(); // ������Ʈ���������� ��ü���. ex) DX113D_2004����.
	igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");

	ModelData modelData;
	mModelDatas.push_back(modelData);

	initLight();
	initCamera();
	initExtensionPreviewImage();
	initRSState();
	initColorPicking();
}

ColliderSettingScene::~ColliderSettingScene()
{
	GM->SafeDelete(mLightBuffer);
	GM->SafeDelete(mDirectionalLight);
	GM->SafeDelete(mWorldCamera);
	GM->SafeDelete(mRSState);
	GM->SafeDelete(mRSStateForColorPicking);
	GM->SafeDelete(mPreRenderTargets[0]);
	GM->SafeDelete(mPreRenderTargetDSV);
	GM->SafeDelete(mInputBuffer);
	GM->SafeDelete(mOutputBuffer);
	GM->SafeDelete(mComputeStructuredBuffer);
	GM->SafeDeleteVector(mModels);

	deleteColliders();
}

void ColliderSettingScene::Update()
{
	updateLight();
	updateCamera();
	colorPicking();
	input();
}

void ColliderSettingScene::PreRender()
{
	renderColorPicking();
}

void ColliderSettingScene::Render()
{
	Device::Get()->ClearRenderTargetView(Float4(0.18f, 0.18f, 0.25f, 1.0f));
	Device::Get()->ClearDepthStencilView();
	Device::Get()->SetRenderTarget();
	Environment::Get()->Set(); 

	mLightBuffer->SetPSBuffer(0);
	mWorldCamera->SetViewBufferToVS();
	mWorldCamera->SetProjectionBufferToVS();
	
	mRSState->SetState();

	renderColliders();
}

void ColliderSettingScene::PostRender()
{
	Environment::Get()->Set(); 
	mWorldCamera->SetViewBufferToVS();
	mWorldCamera->SetProjectionBufferToVS();

	showModelSelectWindow();

	if (mModels.size() != 0) // ToolModel ��������.
	{
		showModelInspector();
		showModelHierarchyWindow();
		showColliderEditorWindow();
	}

	if (mModelList.size() != 0)
	{
		showAssetWindow();
	}

	showPreRenderTargetWindow();

	if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left))
	{
		mDraggedFileName = "";
	}

	if (mCurrentPickedCollider != nullptr)
	{
		renderGizmos();
	}
}

void ColliderSettingScene::moveWorldCamera()
{
	// Update Position
	if (KEY_PRESS(VK_RBUTTON))
	{
		if (KEY_PRESS('I'))
			mWorldCamera->mPosition += mWorldCamera->GetForwardVector() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('K'))
			mWorldCamera->mPosition -= mWorldCamera->GetForwardVector() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('J'))
			mWorldCamera->mPosition -= mWorldCamera->GetRightVector() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('L'))
			mWorldCamera->mPosition += mWorldCamera->GetRightVector() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('U'))
			mWorldCamera->mPosition -= mWorldCamera->GetUpVector() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('O'))
			mWorldCamera->mPosition += mWorldCamera->GetUpVector() * mWorldCamera->mMoveSpeed * DELTA;
	}

	mWorldCamera->mPosition += mWorldCamera->GetForwardVector() * Control::Get()->GetWheel() * mWorldCamera->mWheelSpeed * DELTA;

	// Update Rotation
	if (KEY_PRESS(VK_RBUTTON))
	{
		Vector3 value = MOUSEPOS - mPreFrameMousePosition;

		mWorldCamera->mRotation.x += value.y * mWorldCamera->mRotationSpeed * DELTA;
		mWorldCamera->mRotation.y += value.x * mWorldCamera->mRotationSpeed * DELTA;
	}

	mPreFrameMousePosition = MOUSEPOS;
	mWorldCamera->SetViewMatrixToBuffer();
}

void ColliderSettingScene::showModelSelectWindow()
{
	ImGui::Begin("Select Model");
	ImGuiWindowFlags CollapsingHeader_flag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
	ImGuiTreeNodeFlags TreeNodeEx_flags = ImGuiTreeNodeFlags_None;

	selectModel();

	ImGui::End();
}

void ColliderSettingScene::selectModel() // perFrame
{
	for (int i = 0; i < mModelList.size(); i++) // ImGui ����Ʈ ����.
	{
		mModelTypes[i] = mModelList[i].c_str();
	}

	static ImGuiComboFlags flags = 0;
	const char* combo_label = mModelTypes[mCurrentModelIndex];

	if (ImGui::BeginCombo("Models", combo_label, flags))
	{
		int size = mModelList.size();

		for (int i = 0; i < size; i++)
		{
			const bool is_selected = (mCurrentModelIndex == i); // �����Ѱ� ���� index�� �ؽ�Ʈ���

			if (ImGui::Selectable(mModelTypes[i], is_selected)) // itmes[i]�� ���õ� ���¶��, �� ���⼭ ���� ���ŵȴ��� mCurrentIndex ����.
																// ������ �������������.
			{
				if (i != mCurrentModelIndex) // �ٸ� ���ε����� �����ٸ�!
				{
					// �𵨺���� ������ �̺�Ʈ. 

					// Initialize InspectorWindow Text 
					//mMeshTextOnInspector = "";
					//mMaterialTextOnInspector = "";
				}

				mBeforeModelIndex = mCurrentModelIndex;
				mCurrentModelIndex = i; // currentIndex�� i�� ����. // �⺻ �޺��󺧿� ������Ʈ��ų index��.
			}
		}
		ImGui::EndCombo();
	}

	if (mCurrentModel != nullptr)
	{
		mCurrentModel = mModels[mCurrentModelIndex];
	}

	showCreateModelButton();

	SpacingRepeatedly(3);

	if (mModels.size() != 0) // ToolModel �Ҵ� �Ǿ�� ����. ���̾��Ű �����ؾ��ؼ�
	{
		// �� ���濩�ο� ������� ��� �ʱ�ȭ��������ϴ� ��.

		if (mCurrentModel->GetHasMeshes())
		{
			for (auto it = mModelDatas[mCurrentModelIndex].preprocessedNodesMap.begin(); it != mModelDatas[mCurrentModelIndex].preprocessedNodesMap.end(); it++) // ��ó���� ���� �ʱ�ȭ. 
			{
				it->second.clear();
			}

			treeNodePreProcessing();
		}
	}
}

void ColliderSettingScene::InitModelDatas()
{
	// �� ���� ����� �� �ϵ� '�޽ð� ����� ��' ������ �ʱ�ȭ������ߵ�.

	for (auto it = mModelDatas[mCurrentModelIndex].createdCollidersCheckMap.begin(); it != mModelDatas[mCurrentModelIndex].createdCollidersCheckMap.end(); it++) // ����ݶ��̴� ��������üũ�� �ʱ�ȭ.
	{
		it->second = false;
	}

	for (auto it = mModelDatas[mCurrentModelIndex].nodeNameMap.begin(); it != mModelDatas[mCurrentModelIndex].nodeNameMap.end(); it++)
	{
		it->second = "";
	}

	for (auto it = mModelDatas[mCurrentModelIndex].nodeCollidersMap.begin(); it != mModelDatas[mCurrentModelIndex].nodeCollidersMap.end(); it++) // ���� �����ƴ� �ݶ��̴��� �ʱ�ȭ.
	{
		delete it->second.collider;
		it->second.collider = nullptr;
	}

	mModelDatas[mCurrentModelIndex].nodeCollidersMap.clear();
}

void ColliderSettingScene::initLight()
{
	mLightBuffer = new LightBuffer();
	mDirectionalLight = new Light(LightType::DIRECTIONAL);
	mLightBuffer->Add(mDirectionalLight);
}

void ColliderSettingScene::initCamera()
{
	mWorldCamera = new Camera();
	mWorldCamera->mPosition = { -7.3f, 13.96f, -14.15f };
	mWorldCamera->mRotation = { 0.64f, 0.58f, 0.0f, };
	mWorldCamera->mMoveSpeed = 50.0f;
}

void ColliderSettingScene::initExtensionPreviewImage()
{
	mExtensionPreviewImages["mesh"] = Texture::Add(L"ModelData/Mesh_PreviewImage.png");
	mExtensionPreviewImages["clip"] = Texture::Add(L"ModelData/Clip_PreviewImage.png");
	mExtensionPreviewImages["mat"] = Texture::Add(L"ModelData/Material_PreviewImage.png");
	mExtensionPreviewImages["fbx"] = Texture::Add(L"ModelData/FBX_PreviewImage.png");
	mExtensionPreviewImages["FBX"] = Texture::Add(L"ModelData/FBX_PreviewImage.png");
	mExtensionPreviewImages["txt"] = Texture::Add(L"ModelData/Text_PreviewImage.png");
	mExtensionPreviewImages["default"] = Texture::Add(L"ModelData/DefaultImage.png");
}

void ColliderSettingScene::initRSState()
{
	mRSState = new RasterizerState();
	mRSState->FillMode(D3D11_FILL_WIREFRAME);

	mRSStateForColorPicking = new RasterizerState();
	mRSStateForColorPicking->FillMode(D3D11_FILL_SOLID);
}

void ColliderSettingScene::initColorPicking()
{
	mPreRenderTargets[0] = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT); 
	mPreRenderTargetDSV = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true); 

	// Create ComputeShader
	mColorPickingComputeShader = Shader::AddCS(L"ComputeColorPicking");
	mComputeStructuredBuffer = new ComputeStructuredBuffer(sizeof(ColorPickingOutputBuffer), 1);

	if (mInputBuffer == nullptr)
		mInputBuffer = new ColorPickingInputBuffer();

	mOutputBuffer = new ColorPickingOutputBuffer[1];
}

void ColliderSettingScene::updateLight()
{
	mLightBuffer->Update();
	mDirectionalLight->Update();
}

void ColliderSettingScene::updateCamera()
{
	mWorldCamera->Update();
	moveWorldCamera();
}

void ColliderSettingScene::input()
{
	if (KEY_DOWN(VK_LBUTTON))
	{
		bool hasPickedCollider = false;

		for (auto it = mModelDatas[mCurrentModelIndex].nodeCollidersMap.begin(); it != mModelDatas[mCurrentModelIndex].nodeCollidersMap.end(); it++) // ����� ������ �ö��̴� ����.
		{
			Vector3 colliderHashColor = it->second.collider->GetHashColor();
			Collider* collider = it->second.collider;

			if (mMousePositionColor.IsEqual(colliderHashColor)) // �ö��̴��� ��ŷ�ߴٸ�
			{
				mCurrentPickedCollider = collider;
				updatePickedColliderMatrix();
				mCurrentPickedCollider->SetColor(Float4(1.0f, 1.0f, 0.0f, 1.0f)); // ��ŷ�� �ö��̴��� �����
				hasPickedCollider = true;
			}
			else
			{
				collider->SetColor(Float4(0.0f, 1.0f, 0.0f, 1.0f));
				//initPickedColliderMatrix();
			}
		}

		if (!hasPickedCollider)
		{
			mCurrentPickedCollider = nullptr;
		}
	}

	if (mModels.size() != 0) // �޽��巡�׵������ ToolModel�Ҵ������� ������ƮX.
	{
		if (mCurrentModel->GetHasMeshes())
		{
			mCurrentModel = mModels[mCurrentModelIndex];
			mCurrentModel->Update();

			for (auto it = mModelDatas[mCurrentModelIndex].nodeCollidersMap.begin(); it != mModelDatas[mCurrentModelIndex].nodeCollidersMap.end(); it++)
			{
				Matrix matrix;

				matrix = mCurrentModel->GetTransformByNode(it->first) * (*(mCurrentModel->GetWorldMatrix())); // ex) ���ȳ���� �������

				if (mCurrentPickedCollider != nullptr)
				{
					if (it->second.collider == mCurrentPickedCollider)
					{
						mPickedColliderParentMatrix = matrix;
					}
				}

				it->second.collider->SetParent(&matrix);
				it->second.collider->Update();
			}
		}
	}
}

void ColliderSettingScene::renderColorPicking()
{
	RenderTarget::ClearAndSetWithDSV(mPreRenderTargets, 1, mPreRenderTargetDSV);
	Environment::Get()->Set(); // SetViewport
	mWorldCamera->SetViewBufferToVS();
	mWorldCamera->SetProjectionBufferToVS();

	mRSStateForColorPicking->SetState();

	// �÷���ŷ�� ����Ÿ���ؽ��Ŀ� ����.

	for (auto it = mModelDatas[mCurrentModelIndex].nodeCollidersMap.begin(); it != mModelDatas[mCurrentModelIndex].nodeCollidersMap.end(); it++) // ����� ������ �ö��̴� ����.
	{
		it->second.collider->PreRenderForColorPicking();
	}
}

void ColliderSettingScene::renderColliders()
{
	if (mModels.size() != 0) // �޽��巡�׵������ ToolModel�Ҵ������� ����X.
	{
		if (mCurrentModel->GetHasMeshes())
		{
			mCurrentModel->Render();

			for (auto it = mModelDatas[mCurrentModelIndex].nodeCollidersMap.begin(); it != mModelDatas[mCurrentModelIndex].nodeCollidersMap.end(); it++) // ����� ������ �ö��̴� ����.
			{
				it->second.collider->Render();
			}
		}
	}
}

void ColliderSettingScene::treeNodePreProcessing() // �θ� � ���index�� �ڽ������ִ��� ����.
{
	mNodes = mCurrentModel->GetNodes();

	for (int i = 0; i < mNodes.size(); i++)
	{
		int parent = mNodes[i]->index; // �� ó�� �ε��� �θ�����.

		for (int j = 0; j < mNodes.size(); j++)
		{
			if (mNodes[j]->parent == parent)
			{
				if (mNodes[j]->index == parent)
				{
					continue;
				}

				mModelDatas[mCurrentModelIndex].preprocessedNodesMap[parent].push_back(mNodes[j]->index);
			}
		}
	}
}

void ColliderSettingScene::showCreateModelButton()
{
	if (ImGui::Button("Create Model.."))
		ImGui::OpenPopup("Create Model");

	ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Create Model", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char inputText[128] = "";
		ImGui::InputText("Input ModelName", inputText, IM_ARRAYSIZE(inputText));
		ImGui::Spacing();
		ImGui::RadioButton("StaticMesh", &mbIsSkinnedMesh, 0);
		ImGui::SameLine();

		IndentRepeatedly(6);
		ImGui::RadioButton("SkinnedMesh", &mbIsSkinnedMesh, 1);
		UnIndentRepeatedly(6);

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			// ModelData������ InputText�� �ۼ��� �� �״�� ���� ����. 

			string savePath = "ModelData/";
			string name;

			int i = 0;

			while (inputText[i] != '\0')
			{
				name += inputText[i];
				i++;
			}

			mModelList.push_back(name);
			mModels.push_back(new ToolModel(name));
			mCurrentModelIndex = mModels.size() - 1;
			mCurrentModel = mModels[mCurrentModelIndex];
			mCurrentModel->SetName(name);
			mCurrentModel->SetIsSkinnedMesh(mbIsSkinnedMesh);

			ModelData modelData;
			mModelDatas.push_back(modelData);

			savePath += name;
			savePath += '/';
			CreateFolders(savePath);

			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();

		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			// Cancel �̺�Ʈ�� ���� �����Ծ���.
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void ColliderSettingScene::showModelHierarchyWindow()
{
	ImGui::Begin("Hierachy");
	ImGuiWindowFlags CollapsingHeader_flag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
	ImGuiTreeNodeFlags TreeNodeEx_flags = ImGuiTreeNodeFlags_None;

	if (mCurrentModel->GetHasMeshes()) // Mesh�� ���õǾ��־�� ��屸���� ����.
	{
		if (ImGui::TreeNodeEx("ModelBone", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Selected))
		{
			ImGui::TreePop();

			treeNodeRecurs(0);
		}

		for (auto it = mModelDatas[mCurrentModelIndex].nodeCheckMap.begin(); it != mModelDatas[mCurrentModelIndex].nodeCheckMap.end(); ++it) // ���湮���� �ʱ�ȭ.
		{
			it->second = false;
		}
	}

	ImGui::End();
}

void ColliderSettingScene::treeNodeRecurs(int nodesIndex)
{
	if (mModelDatas[mCurrentModelIndex].nodeCheckMap[mNodes[nodesIndex]->index])
	{
		return;
	}

	ImGui::Indent();
	mModelDatas[mCurrentModelIndex].nodeCheckMap[mNodes[nodesIndex]->index] = true; // ���湮 üũ.
	mModelDatas[mCurrentModelIndex].nodeNameMap[mNodes[nodesIndex]->index] = mNodes[nodesIndex]->name;

	if (ImGui::TreeNodeEx(mNodes[nodesIndex]->name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow)) // ����̸� ���.
	{
		ImGui::SameLine();

		if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_OpenOnArrow))
		{
			const char* colliderTypes[3] = { "SphereCollider","BoxCollider","CapsuleCollider" };
			ImGui::Combo("Colliders", (int*)&mCurrentColliderIndex, colliderTypes, 3);
			ImGui::Text("");

			if (ImGui::Button("Create Collider"))
			{
				if (mModelDatas[mCurrentModelIndex].createdCollidersCheckMap[mNodes[nodesIndex]->index]) {} // �̹� ������������� �ѱ��

				else // �ȸ������������ ������ֱ�.
				{
					mModelDatas[mCurrentModelIndex].createdCollidersCheckMap[mNodes[nodesIndex]->index] = true;

					switch (mCurrentColliderIndex)
					{
					case 0:
						mNodeCollider = new SphereCollider();
						break;

					case 1:
						mNodeCollider = new BoxCollider();
						break;

					case 2:
						mNodeCollider = new CapsuleCollider();
						break;

					default:
						break;
					}

					mNodeCollider->mScale = { 10.0f,10.0f,10.0f }; // �������� �� �ʹ� ������ �Ⱥ����� 10���� ����.

					TreeNodeData treeNodeData;
					treeNodeData.collider = mNodeCollider;
					treeNodeData.nodeName = mNodes[nodesIndex]->name; // ����̸� for binarysave

					mModelDatas[mCurrentModelIndex].nodeCollidersMap[mNodes[nodesIndex]->index] = treeNodeData;
				}
			}

			ImGui::Text("");
			ImGui::TreePop();
		}

		if (mModelDatas[mCurrentModelIndex].preprocessedNodesMap[mNodes[nodesIndex]->index].size() != 0) // �ڽ��� �ִٸ�.
		{
			for (int i = 0; i < mModelDatas[mCurrentModelIndex].preprocessedNodesMap[mNodes[nodesIndex]->index].size(); i++)
			{
				treeNodeRecurs(mModelDatas[mCurrentModelIndex].preprocessedNodesMap[mNodes[nodesIndex]->index][i]);
			}
		}

		ImGui::TreePop();
	}

	ImGui::Unindent();
}

void ColliderSettingScene::showColliderEditorWindow()
{
	ImGui::Begin("ColliderEditor");
	ImGuiWindowFlags CollapsingHeader_flag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
	ImGuiTreeNodeFlags TreeNodeEx_flags = ImGuiTreeNodeFlags_None;

	if (mCurrentModel->GetHasMeshes())
	{
		for (auto it = mModelDatas[mCurrentModelIndex].createdCollidersCheckMap.begin(); it != mModelDatas[mCurrentModelIndex].createdCollidersCheckMap.end(); it++)
		{
			if (it->second) // ���� �Ǿ�������
			{
				string labelName = mModelDatas[mCurrentModelIndex].nodeNameMap[it->first];
				string deleteName = "Delete " + mModelDatas[mCurrentModelIndex].nodeNameMap[it->first];
				string positionLabel = labelName + " Position";
				string rotationLabel = labelName + " Rotation";
				string scaleLabel = labelName + " Scale";
				string tagName = labelName + " Collider";
				Collider* collider = mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider;

				string globalPositionLabel = labelName + " GlobalPosition";
				string globalRotationLabel = labelName + " GlobalRotation";
				string globalScaleLabel = labelName + " GlobalScale";

				ImGui::Text(labelName.c_str());

				if (ImGui::Button(deleteName.c_str())) // DeleteButton
				{
					if (collider == mCurrentPickedCollider) // ���� ��ŷ�� �ö��̴��� ���� �ö��̴����.
					{
						mCurrentPickedCollider = nullptr;
					}

					it->second = false;
					delete mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider;

					mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider = nullptr;
					mModelDatas[mCurrentModelIndex].nodeCollidersMap.erase(it->first);
					continue;
				}

				ImGui::InputText(tagName.c_str(), mModelDatas[mCurrentModelIndex].colliderNameMap[it->first], 100);

				ImGui::Text("");

				ImGui::DragFloat3(positionLabel.c_str(), (float*)&collider->mPosition, 0.1f, -100.0f, 100.0f);
				ImGui::DragFloat3(rotationLabel.c_str(), (float*)&collider->mRotation, 0.1f, -3.141592f, 3.141592f);
				ImGui::DragFloat3(scaleLabel.c_str(), (float*)&collider->mScale, 0.1f, -100.0f, 100.0f);

				SpacingRepeatedly(1);

				ImGui::InputFloat3(globalPositionLabel.c_str(), (float*)&collider->mGlobalPosition);
				ImGui::InputFloat3(globalRotationLabel.c_str(), (float*)&collider->mGlobalRotation);
				ImGui::InputFloat3(globalScaleLabel.c_str(), (float*)&collider->mGlobalScale);

				SpacingRepeatedly(2);

				ImGui::Separator();

				SpacingRepeatedly(2);
			}
		}

		if (ImGui::Button("Save"))
		{
			saveAsBinary(); //�Ǽ��� ������� ��� �ּ�ó��.
		}

		ImGui::SameLine();

		if (ImGui::Button("Load"))
		{
			initCollidersInfo();
			loadCollidersBinaryFile(ToWString(mCurrentModel->GetName()));
		}
	}

	ImGui::End();
}

void ColliderSettingScene::updatePickedColliderMatrix()
{
	//Matrix tempMatrix = *(mCurrentPickedCollider->GetWorldMatrix()); 
	Matrix tempMatrix = mCurrentPickedCollider->GetWorldMatrixValue();
	MatrixToFloatArray(tempMatrix, objectTransformMatrix);
}

void ColliderSettingScene::initPickedColliderMatrix()
{
	Matrix tempMatrix = XMMatrixIdentity();
	MatrixToFloatArray(tempMatrix, objectTransformMatrix);
}

void ColliderSettingScene::showAssetWindow() // ex)ModelData/Mutant���� ��� assets��.
{
	// �̹� mModelsList.size() 1�̻�üũ�ϰ� ���Դ�.

	mAssetsWindowName = mCurrentModel->GetName() + "Assets";

	ImGui::Begin(mAssetsWindowName.c_str());


	if (ImGui::IsWindowHovered())
	{
		GM->Get()->SetIsHoveredAssetBrowserWindow(true);
	}
	else
	{
		GM->Get()->SetIsHoveredAssetBrowserWindow(false);
	}

	if (mbIsDropEvent)
	{
		copyDraggedFile();
		mbIsDropEvent = false;
	}

	if (ImGui::Button("Import")) // FBX���� ����. ExportFBX
	{
		ImGui::OpenPopup("Extractor");
	}

	ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Extractor", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		// �ɼ� ����.
		ImGui::Checkbox("Export Mesh", &mbIsExportMesh);
		ImGui::Checkbox("Export Material", &mbIsExportMaterial);
		ImGui::Checkbox("Export Animation", &mbIsExportAnimation);

		SpacingRepeatedly(3);

		if (ImGui::Button("Select File...")) // Open FileDialog.
		{
			mSelectedFilePath = OpenFileDialog();
		}

		SpacingRepeatedly(2);

		string temp = "Selected File : " + mSelectedFilePath;
		float wrapWidth = 130.0f;

		if (mSelectedFilePath.size() != 0)
		{
			wrapWidth = 280.0f;
			Replace(&mSelectedFilePath, "\\", "/");
		}

		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrapWidth);
		ImGui::Text(temp.c_str(), wrapWidth);
		ImGui::PopTextWrapPos();

		SpacingRepeatedly(2);

		static char inputText[128] = "";

		ImGui::InputText("FileName to Create", inputText, 128, ImGuiInputTextFlags_CharsNoBlank);

		if (ImGui::Button("OK", ImVec2(120, 0))) // �ɼǰ��� �������.
		{
			string fileNameToCreate;

			int i = 0;

			while (inputText[i] != '\0')
			{
				fileNameToCreate += inputText[i];
				i++;
			}

			exportFBX(mSelectedFilePath, fileNameToCreate, mCurrentModel->GetName());
			//thread t1([&]() {exportFBX(mSelectedFilePath, fileNameToCreate, mCurrentModel->GetName()); }); // ���ٽ����� �Ķ���ͳѱ��.

			ImGui::CloseCurrentPopup();
			//t1.join();

			mSelectedFilePath = "";
			mbIsExportMesh = false;
			mbIsExportMaterial = false;
			mbIsExportAnimation = false;
		}

		ImGui::SetItemDefaultFocus();

		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			mSelectedFilePath = "";
			mbIsExportMesh = false;
			mbIsExportMaterial = false;
			mbIsExportAnimation = false;

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::Spacing();
	ImGui::Spacing();

	// mCurrentModelAssets ǥ��.(.mat, .mest, .clip ���)

	// filePreviewImage Render.

	vector<string> fileList;
	loadFileList("\\ModelData\\" + mCurrentModel->GetName(), fileList); // Ȯ���ڱ��� ������ ���ϸ�� ����.
	//mModelAssetsFileList = fileList;

	mStandardCursorPos = ImGui::GetCursorPos(); // 8, 50
	ImVec2 windowSize = ImGui::GetWindowSize();

	//ImageButton Setting

	int frame_padding = 0;
	ImVec2 imageButtonSize = ImVec2(64.0f, 64.0f); // �̹�����ư ũ�⼳��.                     
	ImVec2 imageButtonUV0 = ImVec2(0.0f, 0.0f); // ������̹��� uv��ǥ����.
	ImVec2 imageButtonUV1 = ImVec2(1.0f, 1.0f); // ��ü�� ����ҰŴϱ� 1.
	ImVec4 imageButtonBackGroundColor = ImVec4(0.06f, 0.06f, 0.06f, 0.94f); // ImGuiWindowBackGroundColor.
	ImVec4 imageButtonTintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	float widthPadding = 26.0f;
	float heightPadding = 56.0f;
	float distanceHorizontalGap = widthPadding + imageButtonSize.x; // ����������ġ�������� �� �̹�����ư������ ���ΰŸ�.
	float distanceVerticalGap = heightPadding + imageButtonSize.y; // ����������ġ�������� �� �̹�����ư������ ���ΰŸ�. 
	float distanceTextToImage = 6.0f;
	int currentLineIndex = 0;
	int fileCountPerRow = windowSize.x / distanceHorizontalGap; // ��� ǥ���� ���ϰ���. �������� �����ҰͿ���.

	if (fileCountPerRow < 1)
	{
		fileCountPerRow = 1;
	}

	for (int i = 0; i < fileList.size(); i++)
	{
		if ((i % fileCountPerRow) == 0) // �������� 0 �ƴϿ��� ����.
		{
			if (i != 0)
			{
				mStandardCursorPos.y += distanceVerticalGap;
				currentLineIndex = 0;
			}
		}

		string fileExtension = GetExtension(fileList[i]);
		int checkIndex = -1;

		if (fileExtension == "png")
		{
			string temp = "ModelData/" + mCurrentModel->GetName() + "/" + fileList[i];
			mExtensionPreviewImages[fileExtension] = Texture::Add(ToWString(temp));
		}

		ImVec2 textPosition = { mStandardCursorPos.x + currentLineIndex * distanceHorizontalGap , mStandardCursorPos.y + (imageButtonSize.y + distanceTextToImage) };

		ImGui::SetCursorPosY(mStandardCursorPos.y);
		ImGui::SetCursorPosX(mStandardCursorPos.x + currentLineIndex * distanceHorizontalGap);

		ImGui::ImageButton(mExtensionPreviewImages[fileExtension]->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);

		if (KEY_UP(VK_LBUTTON))
		{
			mbIsDropped = true;
		}

		if (ImGui::IsItemHovered() && mbIsDropped)
		{
			checkIndex = i;
		}

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) // ���� �巡�� �̺�Ʈ. 
		{
			if (i == checkIndex)
			{
				ImGui::SetDragDropPayload("Assets", &i, sizeof(int));
				ImGui::ImageButton(mExtensionPreviewImages[fileExtension]->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor); // �巡�� �� ���, ���콺Ŀ����ġ�� ������ �̸������̹���.
				mDraggedFileName = fileList[i];
				mbIsDropped = false;
			}

			ImGui::EndDragDropSource();
		}

		// fileName TextRender.

		static float wrap_width = 64.0f; // �ؽ�Ʈ�ٹٲ����� ����ũ��.

		ImGui::SetCursorPos(textPosition); // Set TextPosition.
		ImGui::PushTextWrapPos(textPosition.x + wrap_width);
		ImGui::Text(fileList[i].c_str(), wrap_width); // Text Render.
		ImGui::PopTextWrapPos();

		currentLineIndex++;
	}

	ImGui::End();
}

void ColliderSettingScene::showModelInspector()
{
	int frame_padding = 0;
	ImVec2 imageButtonSize = ImVec2(12.0f, 12.0f); // �̹�����ư ũ�⼳��.                     
	ImVec2 imageButtonUV0 = ImVec2(0.0f, 0.0f); // ������̹��� uv��ǥ����.
	ImVec2 imageButtonUV1 = ImVec2(1.0f, 1.0f); // ��ü�� ����ҰŴϱ� 1.
	ImVec4 imageButtonBackGroundColor = ImVec4(0.06f, 0.06f, 0.06f, 0.94f); // ImGuiWindowBackGroundColor.
	ImVec4 imageButtonTintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	static Texture* meshImageButtonTexture = mExtensionPreviewImages["default"];
	static Texture* materialImageButtonTexture = mExtensionPreviewImages["default"];
	static Texture* clipImageButtonTexture = mExtensionPreviewImages["default"];

	ImGui::Begin("Inspector");

	SpacingRepeatedly(2);

	string meshType = "";

	if (mCurrentModel->GetIsSkinnedMesh())
	{
		meshType = "MeshType : SkinnedMesh ";
	}

	else
	{
		meshType = "MeshType : StaticMesh ";
	}

	ImGui::Text(meshType.c_str());

	SpacingRepeatedly(2);

	// Mesh ImageButton and Text Render.
	{
		ImGui::Text("Mesh : ");
		ImGui::SameLine();

		if (GetExtension(mModelDatas[mCurrentModelIndex].meshTextOnInspector) == "mesh")
		{
			meshImageButtonTexture = mExtensionPreviewImages["mesh"];
		}
		else
		{
			meshImageButtonTexture = mExtensionPreviewImages["default"];
		}

		ImGui::ImageButton(meshImageButtonTexture->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);

		ImGui::SameLine();
		ImVec2 meshTextPosition = ImGui::GetCursorPos();

		SpacingRepeatedly(2);

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets"))
			{
				//IM_ASSERT(payload->DataSize == sizeof(int));
				//int payload_n = *(const int*)payload->Data; // source���� �巡���� �̹����� index.
				//mDraggedFileName = mModelAssetsFileList[payload_n];

				if (GetExtension(mDraggedFileName) == "mesh") // mesh������ ����ߴ��� .mat�� ����ߴ��� üũ
				{
					meshImageButtonTexture = mExtensionPreviewImages["mesh"];
					mModelDatas[mCurrentModelIndex].meshTextOnInspector = mDraggedFileName;

					// �޽ú����̺�Ʈ����.

					mCurrentModel->SetMesh(mCurrentModel->GetName(), mDraggedFileName); // �����̸�,�����̸�.
					mCurrentModel->ExecuteSetMeshEvent();

					if (mModelDatas[mCurrentModelIndex].materialTextOnInspector != "") // �޽ú���� ���͸����ʱ�ȭ.
					{
						mModelDatas[mCurrentModelIndex].materialTextOnInspector = "";
						materialImageButtonTexture = Texture::Add(L"ModelData/DefaultImage.png");
					}
				}
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::SetCursorPos(meshTextPosition);
		ImGui::Text(mModelDatas[mCurrentModelIndex].meshTextOnInspector.c_str()); // Text Render.
	}

	SpacingRepeatedly(2);

	// Material ImageButton and Text Render.
	{
		ImGui::Text("Material : ");

		ImGui::SameLine();

		if (GetExtension(mModelDatas[mCurrentModelIndex].materialTextOnInspector) == "mat")
		{
			materialImageButtonTexture = mExtensionPreviewImages["mat"];
		}
		else
		{
			materialImageButtonTexture = mExtensionPreviewImages["default"];
		}

		ImGui::ImageButton(materialImageButtonTexture->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);

		ImGui::SameLine();

		ImVec2 materialTextPosition = ImGui::GetCursorPos();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets"))
			{
				//IM_ASSERT(payload->DataSize == sizeof(int));
				//int payload_n = *(const int*)payload->Data; // source���� �巡���� �̹����� index.
				if (GetExtension(mDraggedFileName) == "mat") // mesh������ ����ߴ��� .mat�� ����ߴ��� üũ
				{
					materialImageButtonTexture = mExtensionPreviewImages["mat"];
					mModelDatas[mCurrentModelIndex].materialTextOnInspector = mDraggedFileName;

					mCurrentModel->SetMaterial(mCurrentModel->GetName(), mDraggedFileName); // �����̸�,�����̸�.
				}
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::SetCursorPos(materialTextPosition);
		ImGui::Text(mModelDatas[mCurrentModelIndex].materialTextOnInspector.c_str());
	}

	SpacingRepeatedly(2);

	if (mCurrentModel->GetHasMeshes())
	{
		ImVec2 clipTargetButtonPosition = ImGui::GetCursorPos();
		ImVec2 wireFrameRadioButtonPosition;

		if (ImGui::TreeNode("Clips"))
		{
			static int selected = -1;
			for (int i = 0; i < mCurrentModel->GetClips().size(); i++)
			{
				char buf[50];
				string t = mCurrentModel->GetClipNames()[i];

				sprintf_s(buf, "%s", t.c_str());

				if (ImGui::Selectable(buf, selected == i))
				{
					selected = i;

					mCurrentModel->PlayClip(i, mCurrentClipSpeed, mCurrentClipTakeTime);
				}
			}

			ImGui::TreePop();
		}

		wireFrameRadioButtonPosition = ImGui::GetCursorPos();
		clipTargetButtonPosition.x += 100.0f;

		// Clip ImageButton Render.
		{
			ImGui::SetCursorPos(clipTargetButtonPosition);

			clipImageButtonTexture = mExtensionPreviewImages["default"];

			ImGui::ImageButton(clipImageButtonTexture->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets"))
				{
					if (GetExtension(mDraggedFileName) == "clip")
					{
						mCurrentModel->SetClip(mCurrentModel->GetName(), mDraggedFileName);
					}
				}
				ImGui::EndDragDropTarget();
			}
		}

		ImGui::SetCursorPos(wireFrameRadioButtonPosition);

		SpacingRepeatedly(2);

		if (ImGui::Button("Play"))
		{
			mCurrentModel->PlayAnimation();
		}

		ImGui::SameLine();

		if (ImGui::Button("Stop"))
		{
			mCurrentModel->StopAnimation();
		}

		SpacingRepeatedly(2);

		static ImGuiSliderFlags flags = ImGuiSliderFlags_None;

		ImGui::SliderInt("Animation Frame", &mCurrentModel->GetFrameBuffer()->data.tweenDesc[0].cur.curFrame, 0, mCurrentModel->GetCurrentClipFrameCount(), "%d", flags);

		SpacingRepeatedly(2);

		ImGui::SliderFloat("Animation Speed", &mCurrentClipSpeed, 0, 10, "%f", flags);
		mCurrentModel->SetCurClipSpeed(mCurrentClipSpeed);

		SpacingRepeatedly(2);

		ImGui::SliderFloat("Animation TakeTime", &mCurrentClipTakeTime, 0, 10, "%f", flags);
		mCurrentModel->SetClipTakeTime(mCurrentClipTakeTime);

		/*mCurrentClipSpeed = 1.0f;
		mCurrentClipTakeTime = 0.2f;*/
		SpacingRepeatedly(2);
	}

	SpacingRepeatedly(2);

	// WireFrame RadioButton Render.

	ImGui::RadioButton("SolidFrame", &mbIsWireFrame, 0);
	ImGui::SameLine();

	IndentRepeatedly(6);
	ImGui::RadioButton("WireFrame", &mbIsWireFrame, 1);

	if (mbIsWireFrame)
	{
		mRSState->FillMode(D3D11_FILL_WIREFRAME);
		mRSState->SetState();
	}
	else
	{
		mRSState->FillMode(D3D11_FILL_SOLID);
		mRSState->SetState();
	}

	UnIndentRepeatedly(6);

	ImGui::End();
}

void ColliderSettingScene::showPreRenderTargetWindow()
{
	ImGui::Begin("PreRenderTarget");

	int frame_padding = 0;
	ImVec2 imageButtonSize = ImVec2(300.0f, 300.0f); // �̹�����ư ũ�⼳��.                     
	ImVec2 imageButtonUV0 = ImVec2(0.0f, 0.0f); // ������̹��� uv��ǥ����.
	ImVec2 imageButtonUV1 = ImVec2(1.0f, 1.0f); // ��ü�� ����ҰŴϱ� 1.
	ImVec4 imageButtonBackGroundColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // ImGuiWindowBackGroundColor.
	ImVec4 imageButtonTintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	ImGui::ImageButton(mPreRenderTargets[0]->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);

	ImGui::End();
}

void ColliderSettingScene::saveAsBinary()
{
	string path = "TextData/";
	string name = mCurrentModel->GetName();

	wstring t = ToWString(path + name + ".map");

	BinaryWriter binaryWriter(t);

	vector<ColliderDataForSave> colliderDatas;

	int createdColliderCount = 0;

	for (auto it = mModelDatas[mCurrentModelIndex].createdCollidersCheckMap.begin(); it != mModelDatas[mCurrentModelIndex].createdCollidersCheckMap.end(); it++)
	{
		if (it->second)
		{
			createdColliderCount++; // � �����Ǿ����� üũ.
		}
	}

	binaryWriter.WriteUInt(createdColliderCount); // ������ �ö��̴�����.

	for (auto it = mModelDatas[mCurrentModelIndex].createdCollidersCheckMap.begin(); it != mModelDatas[mCurrentModelIndex].createdCollidersCheckMap.end(); it++)
	{
		if (it->second)
		{
			binaryWriter.WriteString(mModelDatas[mCurrentModelIndex].colliderNameMap[it->first]); // �ö��̴� �̸�(���� �ۼ��� �̸�)
			binaryWriter.WriteString(mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].nodeName); // �ش� �ö��̴� ����̸�(ex LeftArm)

			UINT colliderType = static_cast<int>(mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider->GetType());
			binaryWriter.WriteUInt(colliderType); // �ö��̴�Ÿ��

			ColliderDataForSave data;
			data.position = mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider->mPosition;
			data.rotation = mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider->mRotation;
			data.scale = mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider->mScale;

			colliderDatas.push_back(data);
		}
	}

	binaryWriter.Byte(colliderDatas.data(), sizeof(ColliderDataForSave) * colliderDatas.size());
	binaryWriter.CloseWriter();

	saveAsCSV();
}

void ColliderSettingScene::loadCollidersBinaryFile(wstring fileName)
{
	wstring temp = L"TextData/" + fileName + L".map";
	BinaryReader binaryReader(temp, mbIsSuccessedLoadFile);

	if (mbIsSuccessedLoadFile)
	{
		UINT colliderCount = binaryReader.ReadUInt();
		int colliderType;

		mTempColliderSRTdatas.resize(colliderCount);
		mTempColliderDatas.resize(colliderCount);

		void* ptr1 = (void*)mTempColliderSRTdatas.data();

		for (int i = 0; i < colliderCount; i++)
		{
			mTempColliderDatas[i].colliderName = binaryReader.ReadString();
			mTempColliderDatas[i].nodeName = binaryReader.ReadString();
			mTempColliderDatas[i].colliderType = binaryReader.ReadUInt();
		}

		binaryReader.Byte(&ptr1, sizeof(TempCollider) * colliderCount);

		for (int i = 0; i < colliderCount; i++)
		{
			mTempColliderDatas[i].position = mTempColliderSRTdatas[i].position;
			mTempColliderDatas[i].rotation = mTempColliderSRTdatas[i].rotation;
			mTempColliderDatas[i].scale = mTempColliderSRTdatas[i].scale;
		}

		// Create Colliders;
		for (int i = 0; i < mTempColliderDatas.size(); i++)
		{
			SettedCollider settedCollider;
			Collider* collider = nullptr;

			switch (mTempColliderDatas[i].colliderType)
			{
			case 0: collider = new BoxCollider();
				break;
			case 1: collider = new SphereCollider();
				break;
			case 2: collider = new CapsuleCollider();
				break;
			default:
				break;
			}

			if (collider != nullptr)
			{
				collider->SetTag(mTempColliderDatas[i].colliderName);
				collider->mPosition = mTempColliderDatas[i].position;
				collider->mRotation = mTempColliderDatas[i].rotation;
				collider->mScale = mTempColliderDatas[i].scale;

				settedCollider.colliderName = mTempColliderDatas[i].colliderName;
				settedCollider.nodeName = mTempColliderDatas[i].nodeName;
				settedCollider.collider = collider;

				TreeNodeData treeNodeData;
				treeNodeData.collider = settedCollider.collider;
				treeNodeData.nodeName = settedCollider.nodeName;
				int nodeIndex = mCurrentModel->GetNodeLookupTable()[settedCollider.nodeName];

				mModelDatas[mCurrentModelIndex].nodeNameMap[nodeIndex] = settedCollider.nodeName;
				mModelDatas[mCurrentModelIndex].nodeCollidersMap[nodeIndex] = treeNodeData;
				mModelDatas[mCurrentModelIndex].createdCollidersCheckMap[nodeIndex] = true;
				strcpy_s(mModelDatas[mCurrentModelIndex].colliderNameMap[nodeIndex], settedCollider.colliderName.c_str());
			}
		}
	}

	else // Failed LoadFile
	{
		// �ö��̴�������â�� �ؽ�Ʈ�� ���и޽����� ����??
	}

	binaryReader.CloseReader();
}

void ColliderSettingScene::saveAsCSV()
{
	FILE* file;

	string str = "TextData/Saved" + mCurrentModel->GetName() + "Colliders.csv";
	const char* fileName = str.c_str();

	fopen_s(&file, fileName, "w");

	fprintf( // �÷���
		file,
		"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
		"ColliderName", "",
		"NodeName", "",
		"ColliderType", "",
		"Position.x", "Position.y", "Position.z", "",
		"Rotation.x", "Rotation.y", "Rotation.z", "",
		"Scale.x", "Scale.y", "Scale.z"
	);

	fprintf( // �ٹٲ޿�
		file,
		"\n"
	);

	for (auto it = mModelDatas[mCurrentModelIndex].createdCollidersCheckMap.begin(); it != mModelDatas[mCurrentModelIndex].createdCollidersCheckMap.end(); it++)
	{
		if (it->second)
		{
			ColliderDataForSave data; // �ö��̴� SRT��.
			data.position = mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider->mPosition;
			data.rotation = mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider->mRotation;
			data.scale = mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider->mScale;

			string t = mModels[mCurrentModelIndex]->GetName();
			const char* modelName = t.c_str();
			string colliderName = mModelDatas[mCurrentModelIndex].colliderNameMap[it->first]; // ���� �ۼ��� �ö��̴� �̸�. 
			string nodeName = mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].nodeName; // �ش� �ö��̴��� ��ġ�� ����̸� ex) LeftArm
			string colliderType = "";

			switch (mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider->GetType())
			{
			case eColliderType::BOX:
				colliderType = "Box Collider";
				break;
			case eColliderType::SPHERE:
				colliderType = "Sphere Collider";
				break;
			case eColliderType::CAPSULE:
				colliderType = "Capsule Collider";
				break;
			}


			fprintf(
				file,
				"%s,%s,%s,%s,%s,%s,%.3f,%.3f,%.3f,%s, %.3f,%.3f,%.3f,%s, %.3f,%.3f,%.3f\n",
				colliderName.c_str(), "", nodeName.c_str(), "", colliderType.c_str(), "",
				data.position.x, data.position.y, data.position.z, "",
				data.rotation.x, data.rotation.y, data.rotation.z, "",
				data.scale.x, data.scale.y, data.scale.z
			);
		}
	}

	fclose(file);
}

void ColliderSettingScene::allSaveAsBinary() // ���� ������ ��� ���� �ö��̴� ������ ����.
{
	for (int i = 0; i < mModels.size(); i++)
	{
		string path = "TextData/";
		string name = mModels[i]->GetName();

		wstring t = ToWString(path + name + ".map");

		BinaryWriter binaryWriter(t);

		vector<ColliderDataForSave> colliderDatas;

		int createdColliderCount = 0;

		for (auto it = mModelDatas[i].createdCollidersCheckMap.begin(); it != mModelDatas[i].createdCollidersCheckMap.end(); it++)
		{
			if (it->second)
			{
				createdColliderCount++; // � �����Ǿ����� üũ.
			}
		}

		binaryWriter.WriteUInt(createdColliderCount); // ������ �ö��̴�����.

		for (auto it = mModelDatas[i].createdCollidersCheckMap.begin(); it != mModelDatas[i].createdCollidersCheckMap.end(); it++)
		{
			if (it->second)
			{
				binaryWriter.WriteString(mModelDatas[i].colliderNameMap[it->first]); // �ö��̴� �̸�(���� �ۼ��� �̸�)
				binaryWriter.WriteString(mModelDatas[i].nodeCollidersMap[it->first].nodeName); // �ش� �ö��̴� ����̸�(ex LeftArm)

				ColliderDataForSave data;
				data.position = mModelDatas[i].nodeCollidersMap[it->first].collider->mPosition;
				data.rotation = mModelDatas[i].nodeCollidersMap[it->first].collider->mRotation;
				data.scale = mModelDatas[i].nodeCollidersMap[it->first].collider->mScale;
				// ������� �ö��̴������� �����ؾߵ�, �ڽ����� ���Ǿ�����
				colliderDatas.push_back(data);
			}
		}

		binaryWriter.Byte(colliderDatas.data(), sizeof(ColliderDataForSave) * colliderDatas.size());
		binaryWriter.CloseWriter();
	}

	allSaveAsCSV(); // ���⽱�� CSV�� ���嵵 ����.
}

void ColliderSettingScene::allSaveAsCSV()
{
	for (int i = 0; i < mModels.size(); i++)
	{
		FILE* file;

		string str = "TextData/Saved" + mModels[i]->GetName() + "Colliders.csv";
		const char* fileName = str.c_str();

		fopen_s(&file, fileName, "w");

		fprintf( // �÷���
			file,
			"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
			"ColliderName", "",
			"NodeName", "",
			"Position.x", "Position.y", "Position.z", "",
			"Rotation.x", "Rotation.y", "Rotation.z", "",
			"Scale.x", "Scale.y", "Scale.z"
		);

		fprintf( // �ٹٲ޿�
			file,
			"\n"
		);

		for (auto it = mModelDatas[i].createdCollidersCheckMap.begin(); it != mModelDatas[i].createdCollidersCheckMap.end(); it++)
		{
			if (it->second)
			{
				ColliderDataForSave data; // �ö��̴� SRT��.
				data.position = mModelDatas[i].nodeCollidersMap[it->first].collider->mPosition;
				data.rotation = mModelDatas[i].nodeCollidersMap[it->first].collider->mRotation;
				data.scale = mModelDatas[i].nodeCollidersMap[it->first].collider->mScale;

				string t = mModels[i]->GetName();
				const char* modelName = t.c_str();
				string colliderName = mModelDatas[i].colliderNameMap[it->first]; // ���� �ۼ��� �ö��̴� �̸�. 
				string nodeName = mModelDatas[i].nodeCollidersMap[it->first].nodeName; // �ش� �ö��̴��� ��ġ�� ����̸� ex) LeftArm

				fprintf(
					file,
					"%s,%s,%s,%s,%.3f,%.3f,%.3f,%s, %.3f,%.3f,%.3f,%s, %.3f,%.3f,%.3f\n",
					colliderName.c_str(), "", nodeName.c_str(), "",
					data.position.x, data.position.y, data.position.z, "",
					data.rotation.x, data.rotation.y, data.rotation.z, "",
					data.scale.x, data.scale.y, data.scale.z
				);
			}
		}

		fclose(file);
	}

}

void ColliderSettingScene::exportFBX(string SelectedFilePath, string fileNameToCreate, string parentFolderName)
{
	ModelExporter* exporter = new ModelExporter(SelectedFilePath);

	if (mbIsExportAnimation)
	{
		exporter->ExportClip(fileNameToCreate, parentFolderName); // ���� �ۼ���  �����̸�, �����̸�.
	}
	else
	{
		if (mbIsExportMesh)
		{
			exporter->ExportMesh(fileNameToCreate, parentFolderName);
		}
		if (mbIsExportMaterial)
		{
			exporter->ExportMaterial(fileNameToCreate, parentFolderName);
		}
	}

	GM->SafeDelete(exporter);
}

void ColliderSettingScene::playAssetWindowDropEvent()
{
	mbIsDropEvent = true;
}

void ColliderSettingScene::copyDraggedFile()
{
	vector<wstring> draggedFileList = GM->GetDraggedFileList();

	for (int i = 0; i < draggedFileList.size(); i++)
	{
		string assetsFolderPath = mProjectPath + "\\ModelData\\" + mCurrentModel->GetName() + "\\";
		string fileName = ToString(draggedFileList[i]);
		fileName = GetFileName(fileName);
		assetsFolderPath += fileName;
		BOOL bCopy = ::CopyFile(draggedFileList[i].c_str(), ToWString(assetsFolderPath).c_str(), FALSE);
	}
}

void ColliderSettingScene::initCollidersInfo()
{
	mModelDatas[mCurrentModelIndex].createdCollidersCheckMap.clear();

	for (auto it = mModelDatas[mCurrentModelIndex].nodeCollidersMap.begin(); it != mModelDatas[mCurrentModelIndex].nodeCollidersMap.end(); it++)
	{
		if (it->second.collider != nullptr)
		{
			GM->SafeDelete(it->second.collider);
		}
	}

	mModelDatas[mCurrentModelIndex].nodeCollidersMap.clear();
	mModelDatas[mCurrentModelIndex].colliderNameMap.clear();
}

void ColliderSettingScene::loadFileList(string folderName, vector<string>& fileList)
{
	string path = mProjectPath;

	path += folderName + "\\";
	path += "*.*";

	struct _finddata_t fd;
	intptr_t handle;

	int i = 0;

	if ((handle = _findfirst(path.c_str(), &fd)) == -1L)
	{
		// ���� ������ �߻���ų �̺�Ʈ.
	}
	do
	{
		if (i >= 2)
		{
			fd.name;
			string temp(fd.name);
			fileList.push_back(temp);
		}

		i++;
	} while (_findnext(handle, &fd) == 0);

	_findclose(handle);
}

void ColliderSettingScene::colorPicking()
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

void ColliderSettingScene::renderGizmos()
{
	Matrix viewMatrix;
	Matrix projectionMatrix;

	viewMatrix = mWorldCamera->GetViewMatrix();
	projectionMatrix = mWorldCamera->GetProjectionMatrixInUse();

	Float4x4 cameraView;
	Float4x4 cameraProjection;

	XMStoreFloat4x4(&cameraView, viewMatrix);
	XMStoreFloat4x4(&cameraProjection, projectionMatrix);

	float identityMatrix[16] =
	{
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};

	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

	ImGuiIO& io = ImGui::GetIO(); // Ư�� imgui �����찡 �ƴ϶�, ���ι���۷����Ǵ� �����쿡 ���� �ڵ鷯?�����Ű���.

	ImGuizmo::SetOrthographic(true);
	ImGuizmo::BeginFrame();

	if (ImGui::IsKeyPressed(90)) // z Ű
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69)) // e Ű
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) // r Ű
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	ImGuizmo::Manipulate( // ���� ����� �����ڵ�.
		*cameraView.m, // ����� (float*)
		*cameraProjection.m, // ������� (float*)
		mCurrentGizmoOperation, // ImGuizmo::OPERATION::TRANSLATE
		ImGuizmo::LOCAL,
		objectTransformMatrix, // ���� ���� ��ġ�� ����� ������Ű�����ؼ� �θ���ı��� ���� �����Ʈ������ �ֱ��ؾ���.
		NULL
	);

	if (ImGuizmo::IsUsing())
	{
		float worldTranslation[3];
		float worldRotation[3];
		float worldScale[3];

		ImGui::Begin("Gizmo Transform");

		ImGuizmo::DecomposeMatrixToComponents(objectTransformMatrix, worldTranslation, worldRotation, worldScale);

		ImGui::InputFloat3("Global Translation", worldTranslation);
		ImGui::InputFloat3("Glpbal Rptation", worldRotation);
		ImGui::InputFloat3("Global Scale", worldScale);

		SpacingRepeatedly(2);

		XMVECTOR determinant = XMMatrixDeterminant(mPickedColliderParentMatrix);
		Matrix inverseParnetMatrix = XMMatrixInverse(&determinant, mPickedColliderParentMatrix);
		Matrix worldMatrix;
		FloatArrayToMatrix(objectTransformMatrix, worldMatrix); // �ö��̴��� �����Ʈ����.

		Matrix localMatrix;
		localMatrix = worldMatrix * inverseParnetMatrix; // �ö��̴��� ������ǥ���غ���..
		float localFloatArray[16];

		MatrixToFloatArray(localMatrix, localFloatArray);

		float localTranslation[3];
		float localRotation[3];
		float deltaRotation[3];
		float localScale[3];

		ImGuizmo::DecomposeMatrixToComponents(localFloatArray, localTranslation, localRotation, localScale);

		mCurrentPickedCollider->mPosition.x = localTranslation[0];
		mCurrentPickedCollider->mPosition.y = localTranslation[1];
		mCurrentPickedCollider->mPosition.z = localTranslation[2];

		Vector3 radianRotation = Vector3(XMConvertToRadians(localRotation[0]), XMConvertToRadians(localRotation[1]), XMConvertToRadians(localRotation[2]));
		ImGui::InputFloat3("Local Translation", localTranslation);
		ImGui::InputFloat3("Local Rotation", (float*)&radianRotation);
		ImGui::InputFloat3("Local Scale", localScale);

		mCurrentPickedCollider->mRotation = radianRotation;

		mCurrentPickedCollider->mScale.x = localScale[0];
		mCurrentPickedCollider->mScale.y = localScale[1];
		mCurrentPickedCollider->mScale.z = localScale[2];

		mCurrentPickedCollider->SetParent(&mPickedColliderParentMatrix);
		mCurrentPickedCollider->Update();

		ImGui::End();
	}
}

void ColliderSettingScene::deleteColliders()
{
	for (int i = 0; i < mModels.size(); i++)
	{
		for (auto it = mModelDatas[i].nodeCollidersMap.begin(); it != mModelDatas[i].nodeCollidersMap.end(); it++)
		{
			GM->SafeDelete(it->second.collider);
		}
	}
}