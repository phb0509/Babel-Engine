#include "Framework.h"
#include "ColliderSettingScene.h"

ColliderSettingScene::ColliderSettingScene() : currentClipIndex(0), currentModelIndex(0), oldModelIndex(0)
{
	igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", 0, ".");
	projectPath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath();
	igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");

	// 셋팅할 모델들
	modelList = { "Player", "Mutant" };
	CAMERA->moveSpeed = 70.0f;

	for (int i = 0; i < modelList.size(); i++)
	{
		model = new ToolModel(modelList[i]);
		models.push_back(model);
	}

	// 카메라 위치설정.
	CAMERA->position = { -9.4f, 15.5f, -14.8f };
	CAMERA->rotation = { 0.3f, 0.7f, 0.0f };


	// 모델들에 기본적으로 TPose 셋팅.
	for (int i = 0; i < modelList.size(); i++)
	{
		clipsMap[modelList[i]].push_back("TPose0.clip"); // 씬에서 별개로 각 모델이 어떤 클립을 갖고있는지 갱신해줘야됨. 
													// 클립 추가할때마다 메모리해제했다가 새로할당받기 때문에 각 모델의 clips가 초기화됨.
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
	for (int i = 0; i < modelList.size(); i++) // ImGui 리스트 갱신.
	{
		modelTypes[i] = modelList[i].c_str();
	}

	ImGui::Combo("Models", (int*)&currentModelIndex, modelTypes, modelList.size());


	// 모델 변경여부와 상관없이 계속 초기화시켜줘야하는 곳.

	for (auto it = preprocessedNodes.begin(); it != preprocessedNodes.end(); it++) // 전처리된 노드맵 초기화. 
	{
		it->second.clear();
	}


	// 모델이 바뀔때만 초기화해주면 되는곳.

	if (currentModelIndex != oldModelIndex) 
	{
		for (auto it = createdCollidersCheck.begin(); it != createdCollidersCheck.end(); it++) // 노드콜라이더 생성여부체크맵 초기화.
		{
			it->second = false;
		}

		for (auto it = nodeCollidersMap.begin(); it != nodeCollidersMap.end(); it++) // 모델의 생성됐던 콜라이더들 초기화.
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

			for (int i = 0; i < clipsMap[currentModelName].size(); i++) // 클립추가할때마다 새로 할당받은 후 현재모델의 클립을 전부 새로 리드클립.
			{
				string t = currentModelName + "/" + clipsMap[currentModelName][i];
				model->ReadClip(t); // 새로 할당받은 model에 씬에서 자체적으로 관리하는 클립리스트 내용대로 ReadClip
			}

			models[currentModelIndex] = model; // 새로 할당받은 모델 다시 원래 인덱스에 넣어놓기.
		}
		// close
		igfd::ImGuiFileDialog::Instance()->CloseDialog("TextureKey");
	}

	for (int i = 0; i < clipsMap[currentModelName].size(); i++) // imgui의 클립리스트를 clips의 내용물로 계속 갱신.
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

	for (auto it = nodeCheck.begin(); it != nodeCheck.end(); ++it) // 노드방문흔적 초기화.
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
		int parent = nodes[i]->index; // 맨 처음 인덱스 부모로잡고.

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


	if (ImGui::TreeNodeEx(nodes[nodesIndex]->name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow)) // 노드이름 출력.
	{
		ImGui::SameLine();

		if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_OpenOnArrow))
		{
			const char* colliderTypes[3] = { "SphereCollider","BoxCollider","CapsuleCollider" };
			ImGui::Combo("Colliders", (int*)&currentColliderIndex, colliderTypes, 3);
			ImGui::Text("");


			if (ImGui::Button("CreateCollider"))
			{
				if (createdCollidersCheck[nodes[nodesIndex]->index]) {} // 이미 만들어져있으면 넘기기
				else // 안만들어져있으면 만들어주기.
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

		if (preprocessedNodes[nodes[nodesIndex]->index].size() != 0) // 자식이 있다면.
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