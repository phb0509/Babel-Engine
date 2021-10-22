#pragma once

class ColliderSettingScene : public Scene
{
public:

	struct colliderData
	{
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
	};

	ColliderSettingScene();
	~ColliderSettingScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;

	void SelectClip();
	void SelectModel();

	void ShowModelHierarchy();
	void ShowModelSelect();
	void ShowColliderEditor();
	void Save();

	void TreeNodePreProcessing();
	void TreeNodeRecurs(int nodesIndex);

private:
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
	int mOldModelIndex;
	int mCurrentClipIndex;
	int mCurrentColliderIndex;

	map<string, vector<string>> mClipsMap;
	vector<ToolModel*> mModels;
	vector<NodeData*> mNodes;
	vector<Collider*> mNodeColliders;

	map<int, vector<int>> mPreprocessedNodes;
	map<int, bool> mNodeCheck;
	map<int, bool> mCreatedCollidersCheck;
	map<int, Collider*> mNodeCollidersMap;
	map<int, string> mNodeNameMap;
	map<int, char[100]> mColliderTagMap;

	Collider* mNodeCollider;



};