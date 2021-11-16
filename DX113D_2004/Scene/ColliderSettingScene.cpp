#include "Framework.h"
#include "ColliderSettingScene.h"


ColliderSettingScene::ColliderSettingScene() :
	mModel(nullptr),
	mCurrentModel(nullptr),
	mCurrentModelIndex(0),
	mCurrentModelName(""),
	mBeforeModelIndex(0),
	mExtractor(nullptr),
	mbIsDropEvent(false)
{
	GM->Get()->SetWindowDropEvent(bind(&ColliderSettingScene::playAssetsWindowDropEvent, this));

	// ī�޶� ��ġ����.
	TARGETCAMERA->mPosition = { -9.4f, 15.5f, -14.8f };
	TARGETCAMERA->mRotation = { 0.3f, 0.7f, 0.0f };

	igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", 0, ".");
	mProjectPath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath(); // ������Ʈ���������� ��ü���. ex) DX113D_2004����.
	igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");

	// ������ �𵨵�
	mModelList = {};
	TARGETCAMERA->moveSpeed = 70.0f;

	mExtensionPreviewImages["mesh"] = Texture::Add(L"ModelData/Mesh_PreviewImage.png");
	mExtensionPreviewImages["clip"] = Texture::Add(L"ModelData/Clip_PreviewImage.png");
	mExtensionPreviewImages["mat"] = Texture::Add(L"ModelData/Material_PreviewImage.png");
	mExtensionPreviewImages["fbx"] = Texture::Add(L"ModelData/FBX_PreviewImage.png");


	//// mModelList�Ӹ� �ƴ϶� mModels�� �ϴ� �������, AddModel�Ҷ����� �߰�������Ѵ�.
	//for (int i = 0; i < mModelList.size(); i++)
	//{
	//	mModel = new ToolModel(mModelList[i]);
	//	mModels.push_back(mModel);
	//}


	//// �𵨵鿡 �⺻������ TPose ����.
	//for (int i = 0; i < mModelList.size(); i++)
	//{
	//	mClipsMap[mModelList[i]].push_back("TPose0.clip"); // ������ ������ �� ���� � Ŭ���� �����ִ��� ��������ߵ�. 
	//												// Ŭ�� �߰��Ҷ����� �޸������ߴٰ� �����Ҵ�ޱ� ������ �� ���� clips�� �ʱ�ȭ��.
	//	mModels[i]->clips.push_back("TPose0.clip");
	//	mModels[i]->ReadClip(mModelList[i] + "/TPose0.clip");
	//}

}

ColliderSettingScene::~ColliderSettingScene()
{
}

void ColliderSettingScene::Update()
{
	if (mModels.size() != 0) // �޽��巡�׵������ ToolModel�Ҵ������� ������ƮX.
	{
		mCurrentModel = mModels[mCurrentModelIndex];
		mCurrentModelName = mModelList[mCurrentModelIndex];

		mCurrentModel->SetAnimation(mCurrentModel->currentClipIndex);
		mCurrentModel->Update();

		for (auto it = mNodeCollidersMap.begin(); it != mNodeCollidersMap.end(); it++)
		{
			Matrix matrix;

			matrix = mCurrentModel->GetTransformByNode(it->first) * (*(mCurrentModel->GetWorld()));

			it->second.collider->SetParent(&matrix);

			it->second.collider->Update();
		}
	}
}


void ColliderSettingScene::PreRender()
{
}

void ColliderSettingScene::Render()
{
	if (mModels.size() != 0) // �޽��巡�׵������ ToolModel�Ҵ������� ����X.
	{
		mCurrentModel->Render();

		for (auto it = mNodeCollidersMap.begin(); it != mNodeCollidersMap.end(); it++) // ����� ������ �ö��̴� ����.
		{
			it->second.collider->Render();
			//it->second->RenderAxis();
		}
	}
}

void ColliderSettingScene::PostRender()
{
	showModelSelectWindow();

	if (mModels.size() != 0) // �̰� ToolModel�� �ݵ�� �־����...
	{
		showModelHierarchyWindow();
		showColliderEditorWindow();
	}

	if (mModelList.size() != 0)
	{
		showAssetsWindow();
	}
}




void ColliderSettingScene::showModelSelectWindow()
{
	ImGui::Begin("SelectModel");
	ImGuiWindowFlags CollapsingHeader_flag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
	ImGuiTreeNodeFlags TreeNodeEx_flags = ImGuiTreeNodeFlags_None;

	selectModel();
	//SelectClip();

	ImGui::End();
}


void ColliderSettingScene::selectModel() // perFrame
{
	for (int i = 0; i < mModelList.size(); i++) // ImGui ����Ʈ ����.
	{
		mModelTypes[i] = mModelList[i].c_str();
	}

	ImGui::Combo("Models", (int*)&mCurrentModelIndex, mModelTypes, mModelList.size());

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	showAddButton();

	if (mModels.size() != 0)
	{
		// �� ���濩�ο� ������� ��� �ʱ�ȭ��������ϴ� ��.

		for (auto it = mPreprocessedNodes.begin(); it != mPreprocessedNodes.end(); it++) // ��ó���� ���� �ʱ�ȭ. 
		{
			it->second.clear();
		}

		// ���� �ٲ𶧸� �ʱ�ȭ���ָ� �Ǵ°�.

		if (mCurrentModelIndex != mBeforeModelIndex)
		{
			for (auto it = mCreatedCollidersCheck.begin(); it != mCreatedCollidersCheck.end(); it++) // ����ݶ��̴� ��������üũ�� �ʱ�ȭ.
			{
				it->second = false;
			}

			for (auto it = mNodeCollidersMap.begin(); it != mNodeCollidersMap.end(); it++) // ���� �����ƴ� �ݶ��̴��� �ʱ�ȭ.
			{
				delete it->second.collider;
			}
			mNodeCollidersMap.clear();

			for (auto it = mNodeNameMap.begin(); it != mNodeNameMap.end(); it++)
			{
				it->second = "";
			}

			mBeforeModelIndex = mCurrentModelIndex;
		}

		treeNodePreProcessing();
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

				mPreprocessedNodes[parent].push_back(mNodes[j]->index);
			}
		}
	}
}

void ColliderSettingScene::showAddButton()
{
	if (ImGui::Button("AddModel.."))
		ImGui::OpenPopup("AddModel");

	ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("AddModel", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char inputText[128] = "";
		ImGui::InputText("Input ModelName", inputText, IM_ARRAYSIZE(inputText));

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
			//mModels.push_back(new ToolModel(name));
			mCurrentModelIndex = mModels.size() - 1;
			//mCurrentModel = mModels[mCurrentModelIndex];
			mCurrentModelName = name;

			//mCurrentModel->ReadTPoseClip();

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

void ColliderSettingScene::selectClip()
{
	if (ImGui::Button("SelectClip"))
		igfd::ImGuiFileDialog::Instance()->OpenDialog("TextureKey", "Choose Clip", ".clip", ".");

	// display
	if (igfd::ImGuiFileDialog::Instance()->FileDialog("TextureKey"))
	{
		if (igfd::ImGuiFileDialog::Instance()->IsOk == true)
		{
			mPath = igfd::ImGuiFileDialog::Instance()->GetFilePathName();
			mPath = mPath.substr(mProjectPath.size() + 1, mPath.length());
			Replace(&mPath, "\\", "/");
			mPath = mPath.substr(17 + mCurrentModelName.size(), mPath.length());

			mClipsMap[mCurrentModelName].push_back(mPath); // "SmashAttack0.clip" push_back

			int t_currentClipIndex = mCurrentModel->currentClipIndex;

			delete mModels[mCurrentModelIndex];
			mModel = new ToolModel(mCurrentModelName);
			mModel->currentClipIndex = t_currentClipIndex;

			for (int i = 0; i < mClipsMap[mCurrentModelName].size(); i++) // Ŭ���߰��Ҷ����� ���� �Ҵ���� �� ������� Ŭ���� ���� ���� ����Ŭ��.
			{
				string t = mCurrentModelName + "/" + mClipsMap[mCurrentModelName][i];
				mModel->ReadClip(t); // ���� �Ҵ���� model�� ������ ��ü������ �����ϴ� Ŭ������Ʈ ������ ReadClip
			}

			mModels[mCurrentModelIndex] = mModel; // ���� �Ҵ���� �� �ٽ� ���� �ε����� �־����.
		}

		// close
		igfd::ImGuiFileDialog::Instance()->CloseDialog("TextureKey");
	}

	for (int i = 0; i < mClipsMap[mCurrentModelName].size(); i++) // imgui�� Ŭ������Ʈ�� clips�� ���빰�� ��� ����.
	{
		mClipTypes[i] = mClipsMap[mCurrentModelName][i].c_str();
	}

	ImGui::Combo("Clips", (int*)&mCurrentModel->currentClipIndex, mClipTypes, mClipsMap[mCurrentModelName].size());
}


void ColliderSettingScene::showModelHierarchyWindow()
{
	ImGui::Begin("Hierachy");
	ImGuiWindowFlags CollapsingHeader_flag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
	ImGuiTreeNodeFlags TreeNodeEx_flags = ImGuiTreeNodeFlags_None;

	if (ImGui::TreeNodeEx("ModelBone", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Selected))
	{
		ImGui::TreePop();

		treeNodeRecurs(0);
	}

	for (auto it = mNodeCheck.begin(); it != mNodeCheck.end(); ++it) // ���湮���� �ʱ�ȭ.
	{
		it->second = false;
	}

	ImGui::End();
}

void ColliderSettingScene::treeNodeRecurs(int nodesIndex)
{
	if (mNodeCheck[mNodes[nodesIndex]->index])
	{
		return;
	}

	ImGui::Indent();
	mNodeCheck[mNodes[nodesIndex]->index] = true;
	mNodeNameMap[mNodes[nodesIndex]->index] = mNodes[nodesIndex]->name;


	if (ImGui::TreeNodeEx(mNodes[nodesIndex]->name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow)) // ����̸� ���.
	{
		ImGui::SameLine();

		if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_OpenOnArrow))
		{
			const char* colliderTypes[3] = { "SphereCollider","BoxCollider","CapsuleCollider" };
			ImGui::Combo("Colliders", (int*)&mCurrentColliderIndex, colliderTypes, 3);
			ImGui::Text("");


			if (ImGui::Button("CreateCollider"))
			{
				if (mCreatedCollidersCheck[mNodes[nodesIndex]->index]) {} // �̹� ������������� �ѱ��
				else // �ȸ������������ ������ֱ�.
				{
					mCreatedCollidersCheck[mNodes[nodesIndex]->index] = true;

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

					//mNodeColliders.push_back(mNodeCollider); // �Ⱦ����� �ϴ� �ּ�.

					TreeNodeData treeNodeData;
					treeNodeData.collider = mNodeCollider;
					treeNodeData.nodeName = mNodes[nodesIndex]->name; // ����̸� for binarysave

					mNodeCollidersMap[mNodes[nodesIndex]->index] = treeNodeData;
				}
			}

			ImGui::Text("");
			ImGui::TreePop();
		}

		if (mPreprocessedNodes[mNodes[nodesIndex]->index].size() != 0) // �ڽ��� �ִٸ�.
		{
			for (int i = 0; i < mPreprocessedNodes[mNodes[nodesIndex]->index].size(); i++)
			{
				treeNodeRecurs(mPreprocessedNodes[mNodes[nodesIndex]->index][i]);
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

	for (auto it = mCreatedCollidersCheck.begin(); it != mCreatedCollidersCheck.end(); it++)
	{
		if (it->second)
		{
			string labelName = mNodeNameMap[it->first];
			string deleteName = "Delete " + mNodeNameMap[it->first];
			string Position = labelName + " Position";
			string Rotation = labelName + " Rotation";
			string Scale = labelName + " Scale";

			string tagName = labelName + " Collider";

			ImGui::Text(labelName.c_str());

			if (ImGui::Button(deleteName.c_str()))
			{
				it->second = false;
				delete mNodeCollidersMap[it->first].collider;
				mNodeCollidersMap.erase(it->first);
				continue;
			}

			ImGui::InputText(tagName.c_str(), mColliderNameMap[it->first], 100);

			ImGui::Text("");

			ImGui::InputFloat3(Position.c_str(), (float*)&mNodeCollidersMap[it->first].collider->mPosition);
			ImGui::InputFloat3(Rotation.c_str(), (float*)&mNodeCollidersMap[it->first].collider->mRotation);
			ImGui::InputFloat3(Scale.c_str(), (float*)&mNodeCollidersMap[it->first].collider->mScale);
		}
	}


	if (ImGui::Button("Save"))
	{
		save();
	}

	ImGui::End();
}

void ColliderSettingScene::save()
{
	string path = "TextData/";
	string name = mCurrentModelName;

	wstring t = ToWString(path + name + ".map");

	BinaryWriter colliderWriter(t);

	vector<ColliderDataForSave> colliderDatas;

	int tSize = 0;

	for (auto it = mCreatedCollidersCheck.begin(); it != mCreatedCollidersCheck.end(); it++)
	{
		if (it->second)
		{
			tSize++;
		}
	}

	colliderWriter.UInt(tSize); // ������ �ö��̴�����.

	for (auto it = mCreatedCollidersCheck.begin(); it != mCreatedCollidersCheck.end(); it++)
	{
		if (it->second)
		{
			colliderWriter.String(mColliderNameMap[it->first]);
			colliderWriter.String(mNodeCollidersMap[it->first].nodeName);

			ColliderDataForSave data;
			data.position = mNodeCollidersMap[it->first].collider->mPosition;
			data.rotation = mNodeCollidersMap[it->first].collider->mRotation;
			data.scale = mNodeCollidersMap[it->first].collider->mScale;

			colliderDatas.push_back(data);
		}
	}

	colliderWriter.Byte(colliderDatas.data(), sizeof(ColliderDataForSave) * colliderDatas.size());

	printToCSV(); // ���⽱�� CSV�� ���嵵 ����.
}

void ColliderSettingScene::loadFileList(string folderName, vector<string>& fileList)
{
	//string path = "C:\\Users\\pok98\\source\\repos\\DirectX11_3D_Portfolio\\DX113D_2004\\ModelData\\";
	string path = mProjectPath + "\\ModelData\\";

	int a = 0;
	path = path += folderName + "\\";
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
			int a = 0;
		}

		i++;
	} while (_findnext(handle, &fd) == 0);

	_findclose(handle);
}

void ColliderSettingScene::showAssetsWindow() // ex)ModelData/Mutant���� ��� assets��.
{
	// �̹� mModelsList.size() 1�̻�üũ�ϰ� ���Դ�.

	mAssetsWindowName = mCurrentModelName + "Assets";

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

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::Button("Open..."))
		{
			mSelectedFilePath = OpenFileDialog();
			mSelectedFilePath = GetFileNameWithoutExtension(mSelectedFilePath);
		}

		if (ImGui::Button("OK", ImVec2(120, 0))) // �ɼǰ��� �������.
		{
			thread t1([&]() {exportFBX(mSelectedFilePath); }); // ���ٽ����� �Ķ���ͳѱ��.
			ImGui::CloseCurrentPopup();
			t1.join();
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

	ImGui::Spacing();
	ImGui::Spacing();


	// mCurrentModelAssets ǥ��.(.mat, .mest, .clip ���)

	// filePreviewImage Render.

	vector<string> fileList;
	loadFileList(mCurrentModelName, fileList); // Ȯ���ڱ��� ������ ���ϸ�� ����.

	mStandardCursorPos = ImGui::GetCursorPos(); // 8, 50

	ImVec2 windowSize = ImGui::GetWindowSize();

	int standardLineIndex = 8; // ��� ǥ���� ���ϰ���.
	int currentLineIndex = 0;
	float distanceYgap = 120.0f; // ���ϰ� ���ΰŸ�.
	float distanceXgap = 90.0f; // ���ϰ� ���ΰŸ�.
	float distanceTextToImage = 70.0f;

	for (int i = 0; i < fileList.size(); i++)
	{
		if ((i % standardLineIndex) == 0) // �������� 0 �ƴϿ��� ����.
		{
			if (i != 0)
			{
				mStandardCursorPos.y += distanceYgap;
				currentLineIndex = 0;
			}
		}

		int frame_padding = 0;
		ImVec2 size = ImVec2(64.0f, 64.0f); // �̹�����ư ũ�⼳��.                     
		ImVec2 uv0 = ImVec2(0.0f, 0.0f); // ������̹��� uv��ǥ����.
		ImVec2 uv1 = ImVec2(1.0f, 1.0f); // ��ü�� ����ҰŴϱ� 1.
		ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // ������.(Background Color) ����.       
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		string fileExtension = GetExtension(fileList[i]);

		if (fileExtension == "png")
		{
			string temp = "ModelData/" + mCurrentModelName + "/" + fileList[i];
			mExtensionPreviewImages[fileExtension] = Texture::Add(ToWString(temp));
		}

		ImVec2 textPosition = { mStandardCursorPos.x + currentLineIndex * distanceXgap , mStandardCursorPos.y + distanceTextToImage };

		ImGui::SetCursorPosY(mStandardCursorPos.y);
		ImGui::SetCursorPosX(mStandardCursorPos.x + currentLineIndex * distanceXgap);
		ImGui::ImageButton(mExtensionPreviewImages[fileExtension]->GetSRV(), size, uv0, uv1, frame_padding, bg_col, tint_col); // ImageButten Render.


		// fileName TextRender.

		ImGui::SetCursorPos(textPosition); // Set TextPosition.

		static float wrap_width = 64.0f; // �ؽ�Ʈ�ٹٲ����� ����ũ��.

		ImGui::PushTextWrapPos(textPosition.x + wrap_width);
		ImGui::Text(fileList[i].c_str(), wrap_width); // Text Render.
		ImGui::PopTextWrapPos();

		//if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) // ���� �巡�� �̺�Ʈ.
		//{
		//	ImGui::SetDragDropPayload("DND_DEMO_CELL", &i, sizeof(int)); // �巡���� �� �ε���(int��) ���� ����������.
		//	ImGui::EndDragDropSource();
		//}

		currentLineIndex++;
	}

	ImGui::End();
}

void ColliderSettingScene::printToCSV()
{
	FILE* file;

	string str = "TextData/Saved" + mCurrentModelName + "Colliders.csv";
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

	for (auto it = mCreatedCollidersCheck.begin(); it != mCreatedCollidersCheck.end(); it++)
	{
		if (it->second)
		{
			ColliderDataForSave data;
			data.position = mNodeCollidersMap[it->first].collider->mPosition;
			data.rotation = mNodeCollidersMap[it->first].collider->mRotation;
			data.scale = mNodeCollidersMap[it->first].collider->mScale;

			const char* modelName = mCurrentModelName.c_str();

			string colliderName = mColliderNameMap[it->first];
			string nodeName = mNodeCollidersMap[it->first].nodeName;

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

void ColliderSettingScene::exportFBX(string fileName) // Mutant
{
	ModelExporter* exporter = new ModelExporter(fileName);
	exporter->ExportMaterial(fileName);
	exporter->ExportMesh(fileName);
	delete exporter;
}

void ColliderSettingScene::playAssetsWindowDropEvent() // ��¶�� �̰� ����ȴ�.
{
	mbIsDropEvent = true;
}

void ColliderSettingScene::copyDraggedFile()
{
	vector<wstring> draggedFileList = GM->GetDraggedFileList();

	draggedFileList;

	int a = 0;
}
