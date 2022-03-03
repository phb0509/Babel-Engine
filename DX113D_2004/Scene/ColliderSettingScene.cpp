#include "Framework.h"
#include "ColliderSettingScene.h"


ColliderSettingScene::ColliderSettingScene() :
	mModel(nullptr),
	mCurrentModel(nullptr),
	mCurrentModelIndex(0),
	mBeforeModelIndex(0),
	mExtractor(nullptr),
	mbIsDropEvent(false),
	mbIsWireFrame(true),
	mDraggedFileName(""),
	mDroppedFileName(""),
	mbIsDropped(true),
	mCurrentClipSpeed(1.0f),
	mCurrentClipTakeTime(0.2f),
	mPickedCollider(nullptr)
{
	srand(time(NULL));
	// ���ϵ�� �ݹ��Լ� ����.
	GM->Get()->SetWindowDropEvent(bind(&ColliderSettingScene::playAssetsWindowDropEvent, this));
	Environment::Get()->SetIsEnabledTargetCamera(false); // ����ī�޶� ���.

	// ī�޶� ����.
	WORLDCAMERA->mPosition = { -7.3f, 13.96f, -14.15f };
	WORLDCAMERA->mRotation = { 0.64f, 0.58f, 0.0f, };
	WORLDCAMERA->mMoveSpeed = 50.0f;

	igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", 0, ".");
	mProjectPath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath(); // ������Ʈ���������� ��ü���. ex) DX113D_2004����.
	igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");

	mExtensionPreviewImages["mesh"] = Texture::Add(L"ModelData/Mesh_PreviewImage.png");
	mExtensionPreviewImages["clip"] = Texture::Add(L"ModelData/Clip_PreviewImage.png");
	mExtensionPreviewImages["mat"] = Texture::Add(L"ModelData/Material_PreviewImage.png");
	mExtensionPreviewImages["fbx"] = Texture::Add(L"ModelData/FBX_PreviewImage.png");
	mExtensionPreviewImages["txt"] = Texture::Add(L"ModelData/Text_PreviewImage.png");
	mExtensionPreviewImages["default"] = Texture::Add(L"ModelData/DefaultImage.png");

	mRSState = new RasterizerState();
	mRSState->FillMode(D3D11_FILL_WIREFRAME);

	mRSStateForColorPicking = new RasterizerState();
	mRSStateForColorPicking->FillMode(D3D11_FILL_SOLID);

	terrain = new Terrain();
	mMonster = new Warrok();

	mMonster->SetTerrain(terrain);
	mMonster->mPosition = { 200,0,200 };


	// ó���� ����Ʈ ġ������Ƽ� �̸� �־����.

	string tempName = "Mutant";
	mModelList.push_back(tempName);
	mModels.push_back(new ToolModel(tempName));
	mCurrentModelIndex = mModels.size() - 1;
	mCurrentModel = mModels[mCurrentModelIndex];
	mCurrentModel->SetName(tempName);
	mCurrentModel->SetIsSkinnedMesh(true);

	ModelData modelData;
	mModelDatas.push_back(modelData);


	// ColorPicking
	mDepthStencil = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true); // ���̰�
	mRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT); //
	mRenderTargets[0] = mRenderTarget;

	// Create ComputeShader
	mComputeShader = Shader::AddCS(L"ComputeColorPicking");
	mComputeStructuredBuffer = new ComputeStructuredBuffer(sizeof(ColorPickingOutputBuffer), 1);

	if (mInputBuffer == nullptr)
		mInputBuffer = new ColorPickingInputBuffer();

	mOutputBuffer = new ColorPickingOutputBuffer[1];
}

ColliderSettingScene::~ColliderSettingScene()
{
	for (int i = 0; i < mModels.size(); i++)
	{
		if (mModels[i] != nullptr)
		{
			delete mModels[i];
		}
	}
}

void ColliderSettingScene::Update()
{
	if (Environment::Get()->GetIsEnabledTargetCamera())
	{
		Environment::Get()->GetTargetCamera()->Update();
	}

	Environment::Get()->GetWorldCamera()->Update();

	terrain->Update();
	mMonster->Update();

	// ComputeShader For ColorPicking
	int32_t mousePositionX = static_cast<int32_t>(MOUSEPOS.x);
	int32_t mousePositionY = static_cast<int32_t>(MOUSEPOS.y);

	Int2 mousePosition = { mousePositionX,mousePositionY };
	mMouseScreenUVPosition = { MOUSEPOS.x / WIN_WIDTH, MOUSEPOS.y / WIN_HEIGHT ,0.0f };
	mInputBuffer->data.mouseScreenUVPosition = { mMouseScreenUVPosition.x,mMouseScreenUVPosition.y }; // ���콺��ǥ uv��
	mInputBuffer->data.mouseScreenPosition = mousePosition; // ���콺��ǥ uv��

	mComputeShader->Set(); // ����̽��� Set..
	mInputBuffer->SetCSBuffer(1);

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mRenderTarget->GetSRV());
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mComputeStructuredBuffer->GetUAV(), nullptr);
	DEVICECONTEXT->Dispatch(1, 1, 1);

	mComputeStructuredBuffer->Copy(mOutputBuffer, sizeof(ColorPickingOutputBuffer)); // GPU���� ����Ѱ� �޾ƿ�. 
	
	Vector3 mousePositionColor = mOutputBuffer->color;

	for (auto it = mModelDatas[mCurrentModelIndex].nodeCollidersMap.begin(); it != mModelDatas[mCurrentModelIndex].nodeCollidersMap.end(); it++) // ����� ������ �ö��̴� ����.
	{
		Vector3 colliderHashColor = it->second.collider->GetHashColor();

		if (mousePositionColor.IsEqual(colliderHashColor))
		{
			it->second.collider->SetColor(Float4(1.0f, 1.0f, 0.0f, 1.0f));

			if (KEY_DOWN(VK_LBUTTON))
			{
				mPickedCollider = it->second.collider;
			}
		}

		else
		{
			it->second.collider->SetColor(Float4(0.0f, 1.0f, 0.0f, 1.0f));
		}
	}

	if (mPickedCollider != nullptr)
	{
		mPickedCollider->SetColor(Float4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	

	if (mModels.size() != 0) // �޽��巡�׵������ ToolModel�Ҵ������� ������ƮX.
	{
		if (mCurrentModel->GetHasMeshes())
		{
			mCurrentModel = mModels[mCurrentModelIndex];
			//mCurrentModel->SetAnimation(mCurrentModel->GetCurrentClipIndex());
			mCurrentModel->Update();

			for (auto it = mModelDatas[mCurrentModelIndex].nodeCollidersMap.begin(); it != mModelDatas[mCurrentModelIndex].nodeCollidersMap.end(); it++)
			{
				Matrix matrix;

				matrix = mCurrentModel->GetTransformByNode(it->first) * (*(mCurrentModel->GetWorldMatrix())); // ex) ���ȳ���� �������

				it->second.collider->SetParent(&matrix);
				it->second.collider->Update();
			}
		}
	}
}


void ColliderSettingScene::PreRender()
{
	Environment::Get()->Set();
	Environment::Get()->SetPerspectiveProjectionBuffer();
	RenderTarget::ClearAndSetWithDSV(mRenderTargets, 1, mDepthStencil); // RenderTarget Setting.

	mRSStateForColorPicking->FillMode(D3D11_FILL_SOLID);
	mRSStateForColorPicking->SetState();

	for (auto it = mModelDatas[mCurrentModelIndex].nodeCollidersMap.begin(); it != mModelDatas[mCurrentModelIndex].nodeCollidersMap.end(); it++) // ����� ������ �ö��̴� ����.
	{
		it->second.collider->PreRenderForColorPicking();
	}
}

void ColliderSettingScene::Render()
{
	Device::Get()->SetRenderTarget(); // SetMainRenderTarget

	if (Environment::Get()->GetIsEnabledTargetCamera())
	{
		Environment::Get()->GetTargetCamera()->Render();
	}

	Environment::Get()->GetWorldCamera()->Render();
	Environment::Get()->Set();
	Environment::Get()->SetPerspectiveProjectionBuffer();

	mMonster->Render();
	mRSState->SetState();

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

	if (mPickedCollider != nullptr)
	{
		mPickedCollider->RenderGizmos();
	}
}

void ColliderSettingScene::PostRender()
{
	Environment::Get()->SetPerspectiveProjectionBuffer();

	showModelSelectWindow();

	if (mModels.size() != 0) // ToolModel ��������.
	{
		showModelInspector();
		showModelHierarchyWindow();
		showColliderEditorWindow();
	}

	if (mModelList.size() != 0)
	{
		showAssetsWindow();
	}

	showTestWindow();

	if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left))
	{
		mDraggedFileName = "";
	}
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

	if (mModels.size() != 0) // ToolModel �Ҵ� �Ǿ�� ����. ���̾��Ű �����ؾߵǴϱ�...
	{
		// �� ���濩�ο� ������� ��� �ʱ�ȭ��������ϴ� ��.

		if (mCurrentModel->GetHasMeshes())
		{
			for (auto it = mModelDatas[mCurrentModelIndex].preprocessedNodes.begin(); it != mModelDatas[mCurrentModelIndex].preprocessedNodes.end(); it++) // ��ó���� ���� �ʱ�ȭ. 
			{
				it->second.clear();
			}

			treeNodePreProcessing();
		}
	}
}

void ColliderSettingScene::InitializeModelDatas()
{
	// �� ���� ����� �� �ϵ� '�޽ð� ����� ��' ������ �ʱ�ȭ������ߵ�.

	for (auto it = mModelDatas[mCurrentModelIndex].createdCollidersCheck.begin(); it != mModelDatas[mCurrentModelIndex].createdCollidersCheck.end(); it++) // ����ݶ��̴� ��������üũ�� �ʱ�ȭ.
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

				mModelDatas[mCurrentModelIndex].preprocessedNodes[parent].push_back(mNodes[j]->index);
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

		for (auto it = mModelDatas[mCurrentModelIndex].nodeCheck.begin(); it != mModelDatas[mCurrentModelIndex].nodeCheck.end(); ++it) // ���湮���� �ʱ�ȭ.
		{
			it->second = false;
		}
	}

	ImGui::End();
}

void ColliderSettingScene::treeNodeRecurs(int nodesIndex)
{
	if (mModelDatas[mCurrentModelIndex].nodeCheck[mNodes[nodesIndex]->index])
	{
		return;
	}

	ImGui::Indent();
	mModelDatas[mCurrentModelIndex].nodeCheck[mNodes[nodesIndex]->index] = true;
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
				if (mModelDatas[mCurrentModelIndex].createdCollidersCheck[mNodes[nodesIndex]->index]) {} // �̹� ������������� �ѱ��
				else // �ȸ������������ ������ֱ�.
				{
					mModelDatas[mCurrentModelIndex].createdCollidersCheck[mNodes[nodesIndex]->index] = true;

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

		if (mModelDatas[mCurrentModelIndex].preprocessedNodes[mNodes[nodesIndex]->index].size() != 0) // �ڽ��� �ִٸ�.
		{
			for (int i = 0; i < mModelDatas[mCurrentModelIndex].preprocessedNodes[mNodes[nodesIndex]->index].size(); i++)
			{
				treeNodeRecurs(mModelDatas[mCurrentModelIndex].preprocessedNodes[mNodes[nodesIndex]->index][i]);
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
		for (auto it = mModelDatas[mCurrentModelIndex].createdCollidersCheck.begin(); it != mModelDatas[mCurrentModelIndex].createdCollidersCheck.end(); it++)
		{
			if (it->second) // ���� �Ǿ�������
			{
				string labelName = mModelDatas[mCurrentModelIndex].nodeNameMap[it->first];
				string deleteName = "Delete " + mModelDatas[mCurrentModelIndex].nodeNameMap[it->first];
				string Position = labelName + " Position";
				string Rotation = labelName + " Rotation";
				string Scale = labelName + " Scale";
				string tagName = labelName + " Collider";

				ImGui::Text(labelName.c_str());

				if (ImGui::Button(deleteName.c_str())) // DeleteButton
				{
					it->second = false;
					delete mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider;
					mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider = nullptr;
					mModelDatas[mCurrentModelIndex].nodeCollidersMap.erase(it->first);
					continue;
				}

				ImGui::InputText(tagName.c_str(), mModelDatas[mCurrentModelIndex].colliderNameMap[it->first], 100);

				ImGui::Text("");

				ImGui::InputFloat3(Position.c_str(), (float*)&mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider->mPosition);
				ImGui::InputFloat3(Rotation.c_str(), (float*)&mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider->mRotation);
				ImGui::InputFloat3(Scale.c_str(), (float*)&mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider->mScale);

				SpacingRepeatedly(2);
				ImGui::Separator();
				SpacingRepeatedly(2);
			}
		}

		if (ImGui::Button("Save"))
		{
			//saveAsBinary(); //�Ǽ��� ������� ��� �ּ�ó��.
		}

		ImGui::SameLine();
	}

	ImGui::End();
}

void ColliderSettingScene::showAssetsWindow() // ex)ModelData/Mutant���� ��� assets��.
{
	// �̹� mModelsList.size() 1�̻�üũ�ϰ� ���Դ�.

	mAssetsWindowName = mCurrentModel->GetName() + "Assets";

	ImGui::Begin(mAssetsWindowName.c_str());

	if (mbIsDropEvent)
	{
		if (ImGui::IsWindowHovered())
		{
			copyDraggedFile();
		}
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

		if (ImGui::Button("Select File..."))
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
					//mCurrentModel->PlayClip(i, 1.0f, 0.2f);
					mCurrentModel->PlayClip(i,mCurrentClipSpeed,mCurrentClipSpeed);
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

		ImGui::SameLine();

		if (ImGui::Button("Test"))
		{
			mCurrentModel->TestEvent();
		}

		SpacingRepeatedly(2);

		static ImGuiSliderFlags flags = ImGuiSliderFlags_None;
		//int frame = mCurrentModel->GetCurrentClipFrame();
		//int t = mCurrentModel->GetFrameBuffer()->data.tweenDesc[0].cur.curFrame;

		ImGui::SliderInt("Animation Frame", &mCurrentModel->GetFrameBuffer()->data.tweenDesc[0].cur.curFrame, 0, mCurrentModel->GetCurrentClipFrameCount(), "%d", flags);

		SpacingRepeatedly(2);

		//ImGui::SliderFloat("Animation Speed", &mCurrentClipSpeed, 0, 10, "%f", flags);

		//SpacingRepeatedly(2);

		//ImGui::SliderFloat("Animation TakeTime", &mCurrentClipTakeTime, 0, 10, "%f", flags);

		//SpacingRepeatedly(2);
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

void ColliderSettingScene::showTestWindow()
{
	ImGui::Begin("TestWindow");

	/*if (mCurrentModel != nullptr)
	{
		string t = "CurrentModel : " + mCurrentModel->GetName();
		ImGui::Text(t.c_str());

		{
			string t = "CurrentModelIndex : ";
			t += to_string(mCurrentModelIndex);
			ImGui::Text(t.c_str());
		}

		{
			string t = "ClipCount : " + to_string(mCurrentModel->GetClips().size());
			ImGui::Text(t.c_str());
		}

		{
			string t = "IsGetHasMeshes : " + to_string(mCurrentModel->GetHasMeshes());
			ImGui::Text(t.c_str());
		}

		{
			string t = "DraggedFileName : " + mDraggedFileName;
			ImGui::Text(t.c_str());
		}

		{
			string t = "DroppedFileName : " + mDroppedFileName;
			ImGui::Text(t.c_str());
		}

		{
			string t = "IsDropped : " + to_string(mbIsDropped);
			ImGui::Text(t.c_str());
		}

		{
			string t = "SelectedIndex  : " + to_string(mSelectedIndexFromAssets);
			ImGui::Text(t.c_str());
		}


	}

	if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
	{
		ImGui::Text("Mouse Dragging");
	}
	else
	{
		ImGui::Text("No Mouse Dragging");
	}*/

	int frame_padding = 0;
	ImVec2 imageButtonSize = ImVec2(150.0f, 150.0f); // �̹�����ư ũ�⼳��.                     
	ImVec2 imageButtonUV0 = ImVec2(0.0f, 0.0f); // ������̹��� uv��ǥ����.
	ImVec2 imageButtonUV1 = ImVec2(1.0f, 1.0f); // ��ü�� ����ҰŴϱ� 1.
	ImVec4 imageButtonBackGroundColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // ImGuiWindowBackGroundColor.
	ImVec4 imageButtonTintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	ImGui::ImageButton(mRenderTarget->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);

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

	for (auto it = mModelDatas[mCurrentModelIndex].createdCollidersCheck.begin(); it != mModelDatas[mCurrentModelIndex].createdCollidersCheck.end(); it++)
	{
		if (it->second)
		{
			createdColliderCount++; // � �����Ǿ����� üũ.
		}
	}

	binaryWriter.UInt(createdColliderCount); // ������ �ö��̴�����.

	for (auto it = mModelDatas[mCurrentModelIndex].createdCollidersCheck.begin(); it != mModelDatas[mCurrentModelIndex].createdCollidersCheck.end(); it++)
	{
		if (it->second)
		{
			binaryWriter.String(mModelDatas[mCurrentModelIndex].colliderNameMap[it->first]); // �ö��̴� �̸�(���� �ۼ��� �̸�)
			binaryWriter.String(mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].nodeName); // �ش� �ö��̴� ����̸�(ex LeftArm)

			UINT colliderType = static_cast<int>(mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider->GetType());
			binaryWriter.UInt(colliderType); // �ö��̴�Ÿ��

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

	for (auto it = mModelDatas[mCurrentModelIndex].createdCollidersCheck.begin(); it != mModelDatas[mCurrentModelIndex].createdCollidersCheck.end(); it++)
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
			case eType::BOX:
				colliderType = "Box Collider";
				break;
			case eType::SPHERE:
				colliderType = "Sphere Collider";
				break;
			case eType::CAPSULE:
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

		for (auto it = mModelDatas[i].createdCollidersCheck.begin(); it != mModelDatas[i].createdCollidersCheck.end(); it++)
		{
			if (it->second)
			{
				createdColliderCount++; // � �����Ǿ����� üũ.
			}
		}

		binaryWriter.UInt(createdColliderCount); // ������ �ö��̴�����.

		for (auto it = mModelDatas[i].createdCollidersCheck.begin(); it != mModelDatas[i].createdCollidersCheck.end(); it++)
		{
			if (it->second)
			{
				binaryWriter.String(mModelDatas[i].colliderNameMap[it->first]); // �ö��̴� �̸�(���� �ۼ��� �̸�)
				binaryWriter.String(mModelDatas[i].nodeCollidersMap[it->first].nodeName); // �ش� �ö��̴� ����̸�(ex LeftArm)

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

		for (auto it = mModelDatas[i].createdCollidersCheck.begin(); it != mModelDatas[i].createdCollidersCheck.end(); it++)
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

	delete exporter;
}

void ColliderSettingScene::playAssetsWindowDropEvent()
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

void ColliderSettingScene::loadFileList(string folderName, vector<string>& fileList)
{
	//string path = mProjectPath + "\\ModelData\\";
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