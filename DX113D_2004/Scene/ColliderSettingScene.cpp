#include "Framework.h"
#include "ColliderSettingScene.h"

ColliderSettingScene::ColliderSettingScene() : mCurrentClipIndex(0), mCurrentModelIndex(0), mBeforeModelIndex(0)
{
	igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", 0, ".");
	mProjectPath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath();
	igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");

	// ������ �𵨵�
	mModelList = { "Player", "Mutant" };
	TARGETCAMERA->moveSpeed = 70.0f;

	for (int i = 0; i < mModelList.size(); i++)
	{
		mModel = new ToolModel(mModelList[i]);
		mModels.push_back(mModel);
	}

	// ī�޶� ��ġ����.
	TARGETCAMERA->mPosition = { -9.4f, 15.5f, -14.8f };
	TARGETCAMERA->mRotation = { 0.3f, 0.7f, 0.0f };


	// �𵨵鿡 �⺻������ TPose ����.
	for (int i = 0; i < mModelList.size(); i++)
	{
		mClipsMap[mModelList[i]].push_back("TPose0.clip"); // ������ ������ �� ���� � Ŭ���� �����ִ��� ��������ߵ�. 
													// Ŭ�� �߰��Ҷ����� �޸������ߴٰ� �����Ҵ�ޱ� ������ �� ���� clips�� �ʱ�ȭ��.
		mModels[i]->clips.push_back("TPose0.clip");
		mModels[i]->ReadClip(mModelList[i] + "/TPose0.clip");
	}

}

ColliderSettingScene::~ColliderSettingScene()
{
}

void ColliderSettingScene::Update()
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


void ColliderSettingScene::PreRender()
{
}

void ColliderSettingScene::Render()
{
	mCurrentModel->Render();

	for (auto it = mNodeCollidersMap.begin(); it != mNodeCollidersMap.end(); it++)
	{
		it->second.collider->Render();
		//it->second->RenderAxis();
	}
}

void ColliderSettingScene::PostRender()
{
	ShowModelSelect();
	ShowModelHierarchy();
	ShowColliderEditor();
}


void ColliderSettingScene::ShowModelSelect()
{
	ImGui::Begin("SelectModel");
	ImGuiWindowFlags CollapsingHeader_flag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
	ImGuiTreeNodeFlags TreeNodeEx_flags = ImGuiTreeNodeFlags_None;

	SelectModel();
	SelectClip();

	ImGui::End();
}


void ColliderSettingScene::SelectModel()
{
	for (int i = 0; i < mModelList.size(); i++) // ImGui ����Ʈ ����.
	{
		mModelTypes[i] = mModelList[i].c_str();
	}

	ImGui::Combo("Models", (int*)&mCurrentModelIndex, mModelTypes, mModelList.size());


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


	TreeNodePreProcessing();
}

void ColliderSettingScene::SelectClip()
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

			mClipsMap[mCurrentModelName].push_back(mPath);

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


void ColliderSettingScene::ShowModelHierarchy()
{
	ImGui::Begin("Hierachy");
	ImGuiWindowFlags CollapsingHeader_flag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
	ImGuiTreeNodeFlags TreeNodeEx_flags = ImGuiTreeNodeFlags_None;

	if (ImGui::TreeNodeEx("ModelBone", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Selected))
	{
		ImGui::TreePop();

		TreeNodeRecurs(0);
	}

	for (auto it = mNodeCheck.begin(); it != mNodeCheck.end(); ++it) // ���湮���� �ʱ�ȭ.
	{
		it->second = false;
	}

	ImGui::End();
}


void ColliderSettingScene::TreeNodePreProcessing()
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

void ColliderSettingScene::TreeNodeRecurs(int nodesIndex)
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
				TreeNodeRecurs(mPreprocessedNodes[mNodes[nodesIndex]->index][i]);
			}
		}

		ImGui::TreePop();
	}

	ImGui::Unindent();
}



void ColliderSettingScene::ShowColliderEditor()
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
		Save();
	}

	ImGui::End();
}

void ColliderSettingScene::Save()
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

void ColliderSettingScene::printToCSV()
{
	FILE* file;

	string str = "TextData/Saved" + mCurrentModelName + "Colliders.csv";
	const char* fileName = str.c_str();

	fopen_s(&file, fileName, "w");


	fprintf( // �÷���
		file,
		"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", 
		"ColliderName","",
		"NodeName","",
		"Position.x", "Position.y", "Position.z","",
		"Rotation.x", "Rotation.y", "Rotation.z","",
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
				colliderName.c_str(),"",nodeName.c_str(), "",
				data.position.x, data.position.y, data.position.z,"",
				data.rotation.x, data.rotation.y, data.rotation.z,"",
				data.scale.x, data.scale.y, data.scale.z
			);
		}
	}

	fclose(file);


	/*for (UINT i = 0; i < monsters.size(); i++)
	{
		fprintf(
			file,
			"%d,%.3f,%.3f,%.3f\n",
			i,
			monsters[i]->mPosition.x, monsters[i]->mPosition.y, monsters[i]->mPosition.z
		);
	}

	fclose(file);*/
}