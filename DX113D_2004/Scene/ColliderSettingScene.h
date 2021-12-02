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

	struct ModelData
	{
		map<int, vector<int>> preprocessedNodes;
		map<int, bool> nodeCheck;
		map<int, bool> createdCollidersCheck;
		map<int, TreeNodeData> nodeCollidersMap;
		map<int, string> nodeNameMap;
		map<int, char[100]> colliderNameMap;
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
	void showCreateModelButton();
	void InitializeModelDatas();

	void showModelHierarchyWindow();
	void showModelSelectWindow();
	void showColliderEditorWindow();
	void showAssetsWindow();
	void showModelInspector();
	void showTestWindow();
	void save();
	void loadFileList(string folderName, vector<string>& fileList);

	void treeNodePreProcessing();
	void treeNodeRecurs(int nodesIndex);

	void printToCSV();
	void exportFBX(string selectedFilePath, string fileNameToCreate, string parentFolderName);
	void playAssetsWindowDropEvent();
		
	void copyDraggedFile();


private:
	ToolModel* mModel;
	ToolModel* mCurrentModel;

	string mProjectPath;
	string mPath;

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


	map<int, vector<int>> mPreprocessedNodes;
	map<int, bool> mNodeCheck;
	map<int, bool> mCreatedCollidersCheck;
	map<int, TreeNodeData> mNodeCollidersMap;
	map<int, string> mNodeNameMap;
	map<int, char[100]> mColliderNameMap;


	map<string, Texture*> mExtensionPreviewImages;
	Collider* mNodeCollider;

	ModelExporter* mExtractor;
	int mExportSettingIndex;

	bool mbIsExportMesh;
	bool mbIsExportMaterial;
	bool mbIsExportAnimation;

	string mSelectedFilePath;

	ImVec2 mStandardCursorPos;
	bool mbIsHoveredAssetsWindow;
	bool mbIsDropEvent;

	string mAssetsWindowName;
	string mDraggedFileName;

	int mbIsSkinnedMesh;
	RasterizerState* mRSState;
	int mbIsWireFrame;

	vector<ModelData> mModelDatas;
	vector<string> mModelAssetsFileList;

	string mDroppedFileName;
	//int checkIndex = 100000;
	
};