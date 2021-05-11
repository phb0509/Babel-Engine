#include "Framework.h"
#include "ColliderSettingScene.h"

ColliderSettingScene::ColliderSettingScene() : currentClipIndex(0), currentModelIndex(0), oldModelIndex(0)
{
	igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", 0, ".");
	projectPath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath();
	igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");

	// ������ �𵨵�
	modelList = { "Player", "Mutant" };
	CAMERA->moveSpeed = 70.0f;

	for (int i = 0; i < modelList.size(); i++)
	{
		model = new ToolModel(modelList[i]);
		models.push_back(model);
	}

	// ī�޶� ��ġ����.
	CAMERA->position = { -9.4f, 15.5f, -14.8f };
	CAMERA->rotation = { 0.3f, 0.7f, 0.0f };


	// �𵨵鿡 �⺻������ TPose ����.
	for (int i = 0; i < modelList.size(); i++)
	{
		clipsMap[modelList[i]].push_back("TPose0.clip"); // ������ ������ �� ���� � Ŭ���� �����ִ��� ��������ߵ�. 
													// Ŭ�� �߰��Ҷ����� �޸������ߴٰ� �����Ҵ�ޱ� ������ �� ���� clips�� �ʱ�ȭ��.
		models[i]->clips.push_back("TPose0.clip");
		models[i]->ReadClip(modelList[i] + "/TPose0.clip");
	}

}

ColliderSettingScene::~ColliderSettingScene()
{
}

void ColliderSettingScene::Update()
{
	currentModel = models[currentModelIndex];
	currentModelName = modelList[currentModelIndex];

	currentModel->SetAnimation(currentModel->currentClipIndex);
	currentModel->Update();

	for (auto it = nodeCollidersMap.begin(); it != nodeCollidersMap.end(); it++)
	{
		Matrix matrix;

		matrix = currentModel->GetTransformByNode(it->first) * (*(currentModel->GetWorld()));

		it->second->SetParent(&matrix);

		it->second->Update();
	}
}


void ColliderSettingScene::PreRender()
{
}

void ColliderSettingScene::Render()
{
	currentModel->Render();

	for (auto it = nodeCollidersMap.begin(); it != nodeCollidersMap.end(); it++)
	{
		it->second->Render();
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
	for (int i = 0; i < modelList.size(); i++) // ImGui ����Ʈ ����.
	{
		modelTypes[i] = modelList[i].c_str();
	}

	ImGui::Combo("Models", (int*)&currentModelIndex, modelTypes, modelList.size());


	// �� ���濩�ο� ������� ��� �ʱ�ȭ��������ϴ� ��.

	for (auto it = preprocessedNodes.begin(); it != preprocessedNodes.end(); it++) // ��ó���� ���� �ʱ�ȭ. 
	{
		it->second.clear();
	}


	// ���� �ٲ𶧸� �ʱ�ȭ���ָ� �Ǵ°�.

	if (currentModelIndex != oldModelIndex) 
	{
		for (auto it = createdCollidersCheck.begin(); it != createdCollidersCheck.end(); it++) // ����ݶ��̴� ��������üũ�� �ʱ�ȭ.
		{
			it->second = false;
		}

		for (auto it = nodeCollidersMap.begin(); it != nodeCollidersMap.end(); it++) // ���� �����ƴ� �ݶ��̴��� �ʱ�ȭ.
		{
			delete it->second;
		}
		nodeCollidersMap.clear();

		for (auto it = nodeNamesMap.begin(); it != nodeNamesMap.end(); it++)
		{
			it->second = "";
		}

		oldModelIndex = currentModelIndex;
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
			path = igfd::ImGuiFileDialog::Instance()->GetFilePathName();
			path = path.substr(projectPath.size() + 1, path.length());
			Replace(&path, "\\", "/");
			path = path.substr(17 + currentModelName.size(), path.length());

			clipsMap[currentModelName].push_back(path);

			int t_currentClipIndex = currentModel->currentClipIndex;

			delete models[currentModelIndex];
			model = new ToolModel(currentModelName);
			model->currentClipIndex = t_currentClipIndex;

			for (int i = 0; i < clipsMap[currentModelName].size(); i++) // Ŭ���߰��Ҷ����� ���� �Ҵ���� �� ������� Ŭ���� ���� ���� ����Ŭ��.
			{
				string t = currentModelName + "/" + clipsMap[currentModelName][i];
				model->ReadClip(t); // ���� �Ҵ���� model�� ������ ��ü������ �����ϴ� Ŭ������Ʈ ������ ReadClip
			}

			models[currentModelIndex] = model; // ���� �Ҵ���� �� �ٽ� ���� �ε����� �־����.
		}
		// close
		igfd::ImGuiFileDialog::Instance()->CloseDialog("TextureKey");
	}

	for (int i = 0; i < clipsMap[currentModelName].size(); i++) // imgui�� Ŭ������Ʈ�� clips�� ���빰�� ��� ����.
	{
		clipTypes[i] = clipsMap[currentModelName][i].c_str();
	}

	ImGui::Combo("Clips", (int*)&currentModel->currentClipIndex, clipTypes, clipsMap[currentModelName].size());
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

	for (auto it = nodeCheck.begin(); it != nodeCheck.end(); ++it) // ���湮���� �ʱ�ȭ.
	{
		it->second = false;
	}

	ImGui::End();
}







void ColliderSettingScene::TreeNodePreProcessing()
{
	nodes = currentModel->GetNodes();

	for (int i = 0; i < nodes.size(); i++)
	{
		int parent = nodes[i]->index; // �� ó�� �ε��� �θ�����.

		for (int j = 0; j < nodes.size(); j++)
		{
			if (nodes[j]->parent == parent)
			{
				if (nodes[j]->index == parent)
				{
					continue;
				}

				preprocessedNodes[parent].push_back(nodes[j]->index);
			}
		}
	}
}

void ColliderSettingScene::TreeNodeRecurs(int nodesIndex)
{
	if (nodeCheck[nodes[nodesIndex]->index])
	{
		return;
	}

	ImGui::Indent();
	nodeCheck[nodes[nodesIndex]->index] = true;
	nodeNamesMap[nodes[nodesIndex]->index] = nodes[nodesIndex]->name;


	if (ImGui::TreeNodeEx(nodes[nodesIndex]->name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow)) // ����̸� ���.
	{
		ImGui::SameLine();

		if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_OpenOnArrow))
		{
			const char* colliderTypes[3] = { "SphereCollider","BoxCollider","CapsuleCollider" };
			ImGui::Combo("Colliders", (int*)&currentColliderIndex, colliderTypes, 3);
			ImGui::Text("");


			if (ImGui::Button("CreateCollider"))
			{
				if (createdCollidersCheck[nodes[nodesIndex]->index]) {} // �̹� ������������� �ѱ��
				else // �ȸ������������ ������ֱ�.
				{
					createdCollidersCheck[nodes[nodesIndex]->index] = true;
					switch (currentColliderIndex)
					{
					case 0:
						nodeCollider = new SphereCollider();
						break;

					case 1:
						nodeCollider = new BoxCollider();
						break;

					case 2:
						nodeCollider = new CapsuleCollider();
						break;

					default:
						break;
					}
					
					nodeCollider->scale = { 10.0f,10.0f,10.0f };
					nodeCollidersMap[nodes[nodesIndex]->index] = nodeCollider;
					nodeColliders.push_back(nodeCollider);
				}
			}

			ImGui::Text("");
			ImGui::TreePop();
		}

		if (preprocessedNodes[nodes[nodesIndex]->index].size() != 0) // �ڽ��� �ִٸ�.
		{
			for (int i = 0; i < preprocessedNodes[nodes[nodesIndex]->index].size(); i++)
			{
				TreeNodeRecurs(preprocessedNodes[nodes[nodesIndex]->index][i]);
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

	for (auto it = createdCollidersCheck.begin(); it != createdCollidersCheck.end(); it++)
	{
		if (it->second)
		{
			string labelName = nodeNamesMap[it->first];
			string deleteName = "Delete " + nodeNamesMap[it->first];
			string Position = labelName + " Position";
			string Rotation = labelName + " Rotation";
			string Scale = labelName + " Scale";
			
			string tagName = labelName + " Collider";

			ImGui::Text(labelName.c_str());

			if (ImGui::Button(deleteName.c_str()))
			{
				it->second = false;
				delete nodeCollidersMap[it->first];
				nodeCollidersMap.erase(it->first);
				continue;
			}

			ImGui::InputText(tagName.c_str(), colliderTagMap[it->first], 100);

			ImGui::Text("");

			ImGui::InputFloat3(Position.c_str(), (float*)&nodeCollidersMap[it->first]->position);
			ImGui::InputFloat3(Rotation.c_str(), (float*)&nodeCollidersMap[it->first]->rotation);
			ImGui::InputFloat3(Scale.c_str(), (float*)&nodeCollidersMap[it->first]->scale);
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
	string name = currentModelName;

	wstring t = ToWString(path + name + ".map");

	BinaryWriter colliderWriter(t);

	vector<colliderData> colliderDatas;

	int tSize = 0;

	for (auto it = createdCollidersCheck.begin(); it != createdCollidersCheck.end(); it++)
	{
		if (it->second)
		{
			tSize++;
		}
	}

	colliderWriter.UInt(tSize);

	for (auto it = createdCollidersCheck.begin(); it != createdCollidersCheck.end(); it++)
	{
		if (it->second)
		{
			colliderWriter.String(colliderTagMap[it->first]);

			colliderData data;
			data.position = nodeCollidersMap[it->first]->position;
			data.rotation = nodeCollidersMap[it->first]->rotation;
			data.scale = nodeCollidersMap[it->first]->scale;

			colliderDatas.push_back(data);
		}
	}

	colliderWriter.Byte(colliderDatas.data(), sizeof(colliderData) * colliderDatas.size());
}