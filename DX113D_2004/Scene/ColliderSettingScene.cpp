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
	// 파일드랍 콜백함수 설정.
	GM->Get()->SetWindowDropEvent(bind(&ColliderSettingScene::playAssetsWindowDropEvent, this));
	Environment::Get()->SetIsEnabledTargetCamera(false); // 월드카메라만 사용.

	// 카메라 설정.
	WORLDCAMERA->mPosition = { -7.3f, 13.96f, -14.15f };
	WORLDCAMERA->mRotation = { 0.64f, 0.58f, 0.0f, };
	WORLDCAMERA->mMoveSpeed = 50.0f;

	igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", 0, ".");
	mProjectPath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath(); // 프로젝트폴더까지의 전체경로. ex) DX113D_2004까지.
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


	// 처음에 뮤턴트 치기귀찮아서 미리 넣어놓음.

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
	mDepthStencil = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true); // 깊이값
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
	mInputBuffer->data.mouseScreenUVPosition = { mMouseScreenUVPosition.x,mMouseScreenUVPosition.y }; // 마우스좌표 uv값
	mInputBuffer->data.mouseScreenPosition = mousePosition; // 마우스좌표 uv값

	mComputeShader->Set(); // 디바이스에 Set..
	mInputBuffer->SetCSBuffer(1);

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mRenderTarget->GetSRV());
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mComputeStructuredBuffer->GetUAV(), nullptr);
	DEVICECONTEXT->Dispatch(1, 1, 1);

	mComputeStructuredBuffer->Copy(mOutputBuffer, sizeof(ColorPickingOutputBuffer)); // GPU에서 계산한거 받아옴. 
	
	Vector3 mousePositionColor = mOutputBuffer->color;

	for (auto it = mModelDatas[mCurrentModelIndex].nodeCollidersMap.begin(); it != mModelDatas[mCurrentModelIndex].nodeCollidersMap.end(); it++) // 현재모델 셋팅한 컬라이더 렌더.
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
	

	if (mModels.size() != 0) // 메쉬드래그드랍으로 ToolModel할당전까진 업데이트X.
	{
		if (mCurrentModel->GetHasMeshes())
		{
			mCurrentModel = mModels[mCurrentModelIndex];
			//mCurrentModel->SetAnimation(mCurrentModel->GetCurrentClipIndex());
			mCurrentModel->Update();

			for (auto it = mModelDatas[mCurrentModelIndex].nodeCollidersMap.begin(); it != mModelDatas[mCurrentModelIndex].nodeCollidersMap.end(); it++)
			{
				Matrix matrix;

				matrix = mCurrentModel->GetTransformByNode(it->first) * (*(mCurrentModel->GetWorldMatrix())); // ex) 왼팔노드의 월드행렬

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

	for (auto it = mModelDatas[mCurrentModelIndex].nodeCollidersMap.begin(); it != mModelDatas[mCurrentModelIndex].nodeCollidersMap.end(); it++) // 현재모델 셋팅한 컬라이더 렌더.
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

	if (mModels.size() != 0) // 메쉬드래그드랍으로 ToolModel할당전까진 렌더X.
	{
		if (mCurrentModel->GetHasMeshes())
		{
			mCurrentModel->Render();

			for (auto it = mModelDatas[mCurrentModelIndex].nodeCollidersMap.begin(); it != mModelDatas[mCurrentModelIndex].nodeCollidersMap.end(); it++) // 현재모델 셋팅한 컬라이더 렌더.
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

	if (mModels.size() != 0) // ToolModel 생성이후.
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
	for (int i = 0; i < mModelList.size(); i++) // ImGui 리스트 갱신.
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
			const bool is_selected = (mCurrentModelIndex == i); // 선택한게 현재 index의 텍스트라면

			if (ImGui::Selectable(mModelTypes[i], is_selected)) // itmes[i]가 선택된 상태라면, 즉 여기서 먼저 갱신된다음 mCurrentIndex 갱신.
																 // 갱신은 무조건해줘야함.
			{
				if (i != mCurrentModelIndex) // 다른 모델인덱스를 눌렀다면!
				{
					// 모델변경시 수행할 이벤트. 

					// Initialize InspectorWindow Text 
					//mMeshTextOnInspector = "";
					//mMaterialTextOnInspector = "";
				}

				mBeforeModelIndex = mCurrentModelIndex;
				mCurrentModelIndex = i; // currentIndex에 i값 대입. // 기본 콤보라벨에 업데이트시킬 index임.
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

	if (mModels.size() != 0) // ToolModel 할당 되어야 실행. 하이어라키 렌더해야되니까...
	{
		// 모델 변경여부와 상관없이 계속 초기화시켜줘야하는 곳.

		if (mCurrentModel->GetHasMeshes())
		{
			for (auto it = mModelDatas[mCurrentModelIndex].preprocessedNodes.begin(); it != mModelDatas[mCurrentModelIndex].preprocessedNodes.end(); it++) // 전처리된 노드맵 초기화. 
			{
				it->second.clear();
			}

			treeNodePreProcessing();
		}
	}
}

void ColliderSettingScene::InitializeModelDatas()
{
	// 새 모델을 만들든 뭘 하든 '메시가 변경될 때' 무조건 초기화시켜줘야됨.

	for (auto it = mModelDatas[mCurrentModelIndex].createdCollidersCheck.begin(); it != mModelDatas[mCurrentModelIndex].createdCollidersCheck.end(); it++) // 노드콜라이더 생성여부체크맵 초기화.
	{
		it->second = false;
	}

	for (auto it = mModelDatas[mCurrentModelIndex].nodeNameMap.begin(); it != mModelDatas[mCurrentModelIndex].nodeNameMap.end(); it++)
	{
		it->second = "";
	}

	for (auto it = mModelDatas[mCurrentModelIndex].nodeCollidersMap.begin(); it != mModelDatas[mCurrentModelIndex].nodeCollidersMap.end(); it++) // 모델의 생성됐던 콜라이더들 초기화.
	{
		delete it->second.collider;
		it->second.collider = nullptr;
	}

	mModelDatas[mCurrentModelIndex].nodeCollidersMap.clear();
}

void ColliderSettingScene::treeNodePreProcessing() // 부모에 어떤 노드index가 자식으로있는지 세팅.
{
	mNodes = mCurrentModel->GetNodes();

	for (int i = 0; i < mNodes.size(); i++)
	{
		int parent = mNodes[i]->index; // 맨 처음 인덱스 부모로잡고.

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
			// ModelData폴더에 InputText에 작성한 값 그대로 폴더 생성. 

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
			// Cancel 이벤트는 딱히 넣을게없다.
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

	if (mCurrentModel->GetHasMeshes()) // Mesh가 셋팅되어있어야 노드구조가 존재.
	{
		if (ImGui::TreeNodeEx("ModelBone", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Selected))
		{
			ImGui::TreePop();

			treeNodeRecurs(0);
		}

		for (auto it = mModelDatas[mCurrentModelIndex].nodeCheck.begin(); it != mModelDatas[mCurrentModelIndex].nodeCheck.end(); ++it) // 노드방문흔적 초기화.
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

	if (ImGui::TreeNodeEx(mNodes[nodesIndex]->name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow)) // 노드이름 출력.
	{
		ImGui::SameLine();

		if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_OpenOnArrow))
		{
			const char* colliderTypes[3] = { "SphereCollider","BoxCollider","CapsuleCollider" };
			ImGui::Combo("Colliders", (int*)&mCurrentColliderIndex, colliderTypes, 3);
			ImGui::Text("");

			if (ImGui::Button("Create Collider"))
			{
				if (mModelDatas[mCurrentModelIndex].createdCollidersCheck[mNodes[nodesIndex]->index]) {} // 이미 만들어져있으면 넘기기
				else // 안만들어져있으면 만들어주기.
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

					mNodeCollider->mScale = { 10.0f,10.0f,10.0f }; // 생성했을 때 너무 작으면 안보여서 10으로 세팅.

					TreeNodeData treeNodeData;
					treeNodeData.collider = mNodeCollider;
					treeNodeData.nodeName = mNodes[nodesIndex]->name; // 노드이름 for binarysave

					mModelDatas[mCurrentModelIndex].nodeCollidersMap[mNodes[nodesIndex]->index] = treeNodeData;
				}
			}

			ImGui::Text("");
			ImGui::TreePop();
		}

		if (mModelDatas[mCurrentModelIndex].preprocessedNodes[mNodes[nodesIndex]->index].size() != 0) // 자식이 있다면.
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
			if (it->second) // 생성 되어있으면
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
			//saveAsBinary(); //실수로 누를까봐 잠시 주석처리.
		}

		ImGui::SameLine();
	}

	ImGui::End();
}

void ColliderSettingScene::showAssetsWindow() // ex)ModelData/Mutant내의 모든 assets들.
{
	// 이미 mModelsList.size() 1이상체크하고 들어왔다.

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

	if (ImGui::Button("Import")) // FBX파일 추출. ExportFBX
	{
		ImGui::OpenPopup("Extractor");
	}

	ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Extractor", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		// 옵션 선택.
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

		if (ImGui::Button("OK", ImVec2(120, 0))) // 옵션고르고 추출실행.
		{
			string fileNameToCreate;

			int i = 0;

			while (inputText[i] != '\0')
			{
				fileNameToCreate += inputText[i];
				i++;
			}

			exportFBX(mSelectedFilePath, fileNameToCreate, mCurrentModel->GetName());
			//thread t1([&]() {exportFBX(mSelectedFilePath, fileNameToCreate, mCurrentModel->GetName()); }); // 람다식으로 파라미터넘기기.

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

	// mCurrentModelAssets 표시.(.mat, .mest, .clip 등등)

	// filePreviewImage Render.

	vector<string> fileList;
	loadFileList("\\ModelData\\" + mCurrentModel->GetName(), fileList); // 확장자까지 포함한 파일명들 리턴.
	//mModelAssetsFileList = fileList;

	mStandardCursorPos = ImGui::GetCursorPos(); // 8, 50
	ImVec2 windowSize = ImGui::GetWindowSize();


	//ImageButton Setting

	int frame_padding = 0;
	ImVec2 imageButtonSize = ImVec2(64.0f, 64.0f); // 이미지버튼 크기설정.                     
	ImVec2 imageButtonUV0 = ImVec2(0.0f, 0.0f); // 출력할이미지 uv좌표설정.
	ImVec2 imageButtonUV1 = ImVec2(1.0f, 1.0f); // 전체다 출력할거니까 1.
	ImVec4 imageButtonBackGroundColor = ImVec4(0.06f, 0.06f, 0.06f, 0.94f); // ImGuiWindowBackGroundColor.
	ImVec4 imageButtonTintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	float widthPadding = 26.0f;
	float heightPadding = 56.0f;
	float distanceHorizontalGap = widthPadding + imageButtonSize.x; // 렌더시작위치기준으로 각 이미지버튼사이의 가로거리.
	float distanceVerticalGap = heightPadding + imageButtonSize.y; // 렌더시작위치기준으로 각 이미지버튼사이의 세로거리. 
	float distanceTextToImage = 6.0f;
	int currentLineIndex = 0;
	int fileCountPerRow = windowSize.x / distanceHorizontalGap; // 행당 표시할 파일개수. 동적으로 변경할것예정.

	if (fileCountPerRow < 1)
	{
		fileCountPerRow = 1;
	}

	for (int i = 0; i < fileList.size(); i++)
	{
		if ((i % fileCountPerRow) == 0) // 나머지가 0 아니여야 실행.
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

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) // 원본 드래그 이벤트. 
		{
			if (i == checkIndex)
			{
				ImGui::SetDragDropPayload("Assets", &i, sizeof(int));
				ImGui::ImageButton(mExtensionPreviewImages[fileExtension]->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor); // 드래그 할 경우, 마우스커서위치에 렌더될 미리보기이미지.
				mDraggedFileName = fileList[i];
				mbIsDropped = false;
			}

			ImGui::EndDragDropSource();
		}

		// fileName TextRender.

		static float wrap_width = 64.0f; // 텍스트줄바꿈해줄 기준크기.

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
	ImVec2 imageButtonSize = ImVec2(12.0f, 12.0f); // 이미지버튼 크기설정.                     
	ImVec2 imageButtonUV0 = ImVec2(0.0f, 0.0f); // 출력할이미지 uv좌표설정.
	ImVec2 imageButtonUV1 = ImVec2(1.0f, 1.0f); // 전체다 출력할거니까 1.
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
				//int payload_n = *(const int*)payload->Data; // source에서 드래그한 이미지의 index.
				//mDraggedFileName = mModelAssetsFileList[payload_n];

				if (GetExtension(mDraggedFileName) == "mesh") // mesh파일을 드랍했는지 .mat을 드랍했는지 체크
				{
					meshImageButtonTexture = mExtensionPreviewImages["mesh"];
					mModelDatas[mCurrentModelIndex].meshTextOnInspector = mDraggedFileName;

					// 메시변경이벤트구간.

					mCurrentModel->SetMesh(mCurrentModel->GetName(), mDraggedFileName); // 폴더이름,파일이름.
					mCurrentModel->ExecuteSetMeshEvent();

					if (mModelDatas[mCurrentModelIndex].materialTextOnInspector != "") // 메시변경시 머터리얼초기화.
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
				//int payload_n = *(const int*)payload->Data; // source에서 드래그한 이미지의 index.
				if (GetExtension(mDraggedFileName) == "mat") // mesh파일을 드랍했는지 .mat을 드랍했는지 체크
				{
					materialImageButtonTexture = mExtensionPreviewImages["mat"];
					mModelDatas[mCurrentModelIndex].materialTextOnInspector = mDraggedFileName;

					mCurrentModel->SetMaterial(mCurrentModel->GetName(), mDraggedFileName); // 폴더이름,파일이름.
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
	ImVec2 imageButtonSize = ImVec2(150.0f, 150.0f); // 이미지버튼 크기설정.                     
	ImVec2 imageButtonUV0 = ImVec2(0.0f, 0.0f); // 출력할이미지 uv좌표설정.
	ImVec2 imageButtonUV1 = ImVec2(1.0f, 1.0f); // 전체다 출력할거니까 1.
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
			createdColliderCount++; // 몇개 생성되었는지 체크.
		}
	}

	binaryWriter.UInt(createdColliderCount); // 저장할 컬라이더개수.

	for (auto it = mModelDatas[mCurrentModelIndex].createdCollidersCheck.begin(); it != mModelDatas[mCurrentModelIndex].createdCollidersCheck.end(); it++)
	{
		if (it->second)
		{
			binaryWriter.String(mModelDatas[mCurrentModelIndex].colliderNameMap[it->first]); // 컬라이더 이름(직접 작성한 이름)
			binaryWriter.String(mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].nodeName); // 해당 컬라이더 노드이름(ex LeftArm)

			UINT colliderType = static_cast<int>(mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider->GetType());
			binaryWriter.UInt(colliderType); // 컬라이더타입

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

	fprintf( // 컬럼명
		file,
		"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
		"ColliderName", "",
		"NodeName", "",
		"ColliderType", "",
		"Position.x", "Position.y", "Position.z", "",
		"Rotation.x", "Rotation.y", "Rotation.z", "",
		"Scale.x", "Scale.y", "Scale.z"
	);

	fprintf( // 줄바꿈용
		file,
		"\n"
	);

	for (auto it = mModelDatas[mCurrentModelIndex].createdCollidersCheck.begin(); it != mModelDatas[mCurrentModelIndex].createdCollidersCheck.end(); it++)
	{
		if (it->second)
		{
			ColliderDataForSave data; // 컬라이더 SRT값.
			data.position = mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider->mPosition;
			data.rotation = mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider->mRotation;
			data.scale = mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].collider->mScale;

			string t = mModels[mCurrentModelIndex]->GetName();
			const char* modelName = t.c_str();
			string colliderName = mModelDatas[mCurrentModelIndex].colliderNameMap[it->first]; // 직접 작성한 컬라이더 이름. 
			string nodeName = mModelDatas[mCurrentModelIndex].nodeCollidersMap[it->first].nodeName; // 해당 컬라이더가 위치한 노드이름 ex) LeftArm
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

void ColliderSettingScene::allSaveAsBinary() // 현재 생성된 모든 모델의 컬라이더 개별로 저장.
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
				createdColliderCount++; // 몇개 생성되었는지 체크.
			}
		}

		binaryWriter.UInt(createdColliderCount); // 저장할 컬라이더개수.

		for (auto it = mModelDatas[i].createdCollidersCheck.begin(); it != mModelDatas[i].createdCollidersCheck.end(); it++)
		{
			if (it->second)
			{
				binaryWriter.String(mModelDatas[i].colliderNameMap[it->first]); // 컬라이더 이름(직접 작성한 이름)
				binaryWriter.String(mModelDatas[i].nodeCollidersMap[it->first].nodeName); // 해당 컬라이더 노드이름(ex LeftArm)

				ColliderDataForSave data;
				data.position = mModelDatas[i].nodeCollidersMap[it->first].collider->mPosition;
				data.rotation = mModelDatas[i].nodeCollidersMap[it->first].collider->mRotation;
				data.scale = mModelDatas[i].nodeCollidersMap[it->first].collider->mScale;
				// 어떤종류의 컬라이더인지도 저장해야됨, 박스인지 스피어인지
				colliderDatas.push_back(data);
			}
		}

		binaryWriter.Byte(colliderDatas.data(), sizeof(ColliderDataForSave) * colliderDatas.size());
		binaryWriter.CloseWriter();
	}

	allSaveAsCSV(); // 보기쉽게 CSV로 저장도 해줌.
}

void ColliderSettingScene::allSaveAsCSV()
{
	for (int i = 0; i < mModels.size(); i++)
	{
		FILE* file;

		string str = "TextData/Saved" + mModels[i]->GetName() + "Colliders.csv";
		const char* fileName = str.c_str();

		fopen_s(&file, fileName, "w");

		fprintf( // 컬럼명
			file,
			"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
			"ColliderName", "",
			"NodeName", "",
			"Position.x", "Position.y", "Position.z", "",
			"Rotation.x", "Rotation.y", "Rotation.z", "",
			"Scale.x", "Scale.y", "Scale.z"
		);

		fprintf( // 줄바꿈용
			file,
			"\n"
		);

		for (auto it = mModelDatas[i].createdCollidersCheck.begin(); it != mModelDatas[i].createdCollidersCheck.end(); it++)
		{
			if (it->second)
			{
				ColliderDataForSave data; // 컬라이더 SRT값.
				data.position = mModelDatas[i].nodeCollidersMap[it->first].collider->mPosition;
				data.rotation = mModelDatas[i].nodeCollidersMap[it->first].collider->mRotation;
				data.scale = mModelDatas[i].nodeCollidersMap[it->first].collider->mScale;

				string t = mModels[i]->GetName();
				const char* modelName = t.c_str();
				string colliderName = mModelDatas[i].colliderNameMap[it->first]; // 직접 작성한 컬라이더 이름. 
				string nodeName = mModelDatas[i].nodeCollidersMap[it->first].nodeName; // 해당 컬라이더가 위치한 노드이름 ex) LeftArm

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
		exporter->ExportClip(fileNameToCreate, parentFolderName); // 직접 작성한  파일이름, 폴더이름.
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
		// 파일 없을때 발생시킬 이벤트.
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