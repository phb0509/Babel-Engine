#pragma once

class ColliderSettingScene : public Scene
{
public:

	struct ColliderDataForSave
	{
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
	};

	struct TreeNodeData
	{
		Collider* collider;
		string nodeName;
	};

	

	ColliderSettingScene();
	~ColliderSettingScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;

	

private:
	void selectClip();
	void selectModel();
	void showAddButton();

	void showModelHierarchy();
	void showModelSelect();
	void showColliderEditor();
	void showAssets();
	void save();
	void LoadFileList(string folderName);

	void treeNodePreProcessing();
	void treeNodeRecurs(int nodesIndex);

	void printToCSV();
	
	


private:
	ToolModel* mModel;
	ToolModel* mCurrentModel;

	string mProjectPath;
	string mPath;
	string mCurrentModelName;

	vector<string> mClips;
	vector<string> mModelList;

	const char* mClipTypes[20];
	const char* mModelTypes[20];


	int mCurrentModelIndex;
	int mBeforeModelIndex;
	int mCurrentColliderIndex;

	map<string, vector<string>> mClipsMap;
	vector<ToolModel*> mModels;
	vector<NodeData*> mNodes;
	//vector<Collider*> mNodeColliders;

	map<int, vector<int>> mPreprocessedNodes;
	map<int, bool> mNodeCheck;
	map<int, bool> mCreatedCollidersCheck;
	map<int, TreeNodeData> mNodeCollidersMap;
	map<int, string> mNodeNameMap;
	map<int, char[100]> mColliderNameMap;

	Collider* mNodeCollider;

};