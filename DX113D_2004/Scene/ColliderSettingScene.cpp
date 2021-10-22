#include "Framework.h"
#include "ColliderSettingScene.h"

ColliderSettingScene::ColliderSettingScene() : mCurrentClipIndex(0), mCurrentModelIndex(0), mOldModelIndex(0)
{
	igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", 0, ".");
	mProjectPath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath();
	igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");

	// 셋팅할 모델들
	mModelList = { "Player", "Mutant" };
	TARGETCAMERA->moveSpeed = 70.0f;

	for (int i = 0; i < mModelList.size(); i++)
	{
		mModel = new ToolModel(mModelList[i]);
		mModels.push_back(mModel);
	}

	// 카메라 위치설정.
	TARGETCAMERA->mPosition = { -9.4f, 15.5f, -14.8f };
	TARGETCAMERA->mRotation = { 0.3f, 0.7f, 0.0f };


	// 모델들에 기본적으로 TPose 셋팅.
	for (int i = 0; i < mModelList.size(); i++)
	{
		mClipsMap[mModelList[i]].push_back("TPose0.clip"); // 씬에서 별개로 각 모델이 어떤 클립을 갖고있는지 갱신해줘야됨. 
													// 클립 추가할때마다 메모리해제했다가 새로할당받기 때문에 각 모델의 clips가 초기화됨.
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

		it->second->SetParent(&matrix);

		it->second->Update();
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
	for (int i = 0; i < mModelList.size(); i++) // ImGui 리스트 갱신.
	{
		mModelTypes[i] = mModelList[i].c_str();
	}

	ImGui::Combo("Models", (int*)&mCurrentModelIndex, mModelTypes, mModelList.size());


	// 모델 변경여부와 상관없이 계속 초기화시켜줘야하는 곳.

	for (auto it = mPreprocessedNodes.begin(); it != mPreprocessedNodes.end(); it++) // 전처리된 노드맵 초기화. 
	{
		it->second.clear();
	}


	// 모델이 바뀔때만 초기화해주면 되는곳.

	if (mCurrentModelIndex != mOldModelIndex) 
	{
		for (auto it = mCreatedCollidersCheck.begin(); it != mCreatedCollidersCheck.end(); it++) // 노드콜라이더 생성여부체크맵 초기화.
		{
			it->second = false;
		}

		for (auto it = mNodeCollidersMap.begin(); it != mNodeCollidersMap.end(); it++) // 모델의 생성됐던 콜라이더들 초기화.
		{
			delete it->second;
		}
		mNodeCollidersMap.clear();

		for (auto it = mNodeNameMap.begin(); it != mNodeNameMap.end(); it++)
		{
			it->second = "";
		}

		mOldModelIndex = mCurrentModelIndex;
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

			for (int i = 0; i < mClipsMap[mCurrentModelName].size(); i++) // 클립추가할때마다 새로 할당받은 후 현재모델의 클립을 전부 새로 리드클립.
			{
				string t = mCurrentModelName + "/" + mClipsMap[mCurrentModelName][i];
				mModel->ReadClip(t); // 새로 할당받은 model에 씬에서 자체적으로 관리하는 클립리스트 내용대로 ReadClip
			}

			mModels[mCurrentModelIndex] = mModel; // 새로 할당받은 모델 다시 원래 인덱스에 넣어놓기.
		}
		// close
		igfd::ImGuiFileDialog::Instance()->CloseDialog("TextureKey");
	}

	for (int i = 0; i < mClipsMap[mCurrentModelName].size(); i++) // imgui의 클립리스트를 clips의 내용물로 계속 갱신.
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

	for (auto it = mNodeCheck.begin(); it != mNodeCheck.end(); ++it) // 노드방문흔적 초기화.
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
		int parent = mNodes[i]->index; // 맨 처음 인덱스 부모로잡고.

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


	if (ImGui::TreeNodeEx(mNodes[nodesIndex]->name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow)) // 노드이름 출력.
	{
		ImGui::SameLine();

		if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_OpenOnArrow))
		{
			const char* colliderTypes[3] = { "SphereCollider","BoxCollider","CapsuleCollider" };
			ImGui::Combo("Colliders", (int*)&mCurrentColliderIndex, colliderTypes, 3);
			ImGui::Text("");


			if (ImGui::Button("CreateCollider"))
			{
				if (mCreatedCollidersCheck[mNodes[nodesIndex]->index]) {} // 이미 만들어져있으면 넘기기
				else // 안만들어져있으면 만들어주기.
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
					
					mNodeCollider->mScale = { 10.0f,10.0f,10.0f };
					mNodeCollidersMap[mNodes[nodesIndex]->index] = mNodeCollider;
					mNodeColliders.push_back(mNodeCollider);
				}
			}

			ImGui::Text("");
			ImGui::TreePop();
		}

		if (mPreprocessedNodes[mNodes[nodesIndex]->index].size() != 0) // 자식이 있다면.
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

void ColliderSettingScene::printToCSV()
{
	FILE* file;
	const char* fileName;
	//fopen_s(&file, "TextData/Saved" + mCurrentModelName + "Collider.csv", "w");


	for (UINT i = 0; i < monsters.size(); i++)
	{
		fprintf(
			file,
			"%d,%.3f,%.3f,%.3f\n",
			i,
			monsters[i]->mPosition.x, monsters[i]->mPosition.y, monsters[i]->mPosition.z
		);
	}

	fclose(file);
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
				delete mNodeCollidersMap[it->first];
				mNodeCollidersMap.erase(it->first);
				continue;
			}

			ImGui::InputText(tagName.c_str(), mColliderTagMap[it->first], 100);

			ImGui::Text("");

			ImGui::InputFloat3(Position.c_str(), (float*)&mNodeCollidersMap[it->first]->mPosition);
			ImGui::InputFloat3(Rotation.c_str(), (float*)&mNodeCollidersMap[it->first]->mRotation);
			ImGui::InputFloat3(Scale.c_str(), (float*)&mNodeCollidersMap[it->first]->mScale);
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

	vector<colliderData> colliderDatas;

	int tSize = 0;

	for (auto it = mCreatedCollidersCheck.begin(); it != mCreatedCollidersCheck.end(); it++)
	{
		if (it->second)
		{
			tSize++;
		}
	}

	colliderWriter.UInt(tSize); // 저장할 컬라이더개수.

	for (auto it = mCreatedCollidersCheck.begin(); it != mCreatedCollidersCheck.end(); it++)
	{
		if (it->second)
		{
			colliderWriter.String(mColliderTagMap[it->first]);

			colliderData data;
			data.position = mNodeCollidersMap[it->first]->mPosition;
			data.rotation = mNodeCollidersMap[it->first]->mRotation;
			data.scale = mNodeCollidersMap[it->first]->mScale;

			colliderDatas.push_back(data);
		}
	}

	colliderWriter.Byte(colliderDatas.data(), sizeof(colliderData) * colliderDatas.size());

	printToCSV(); // 보기쉽게 CSV로 저장도 해줌.
}