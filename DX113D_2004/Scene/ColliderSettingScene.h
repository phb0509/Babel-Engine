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
		string nodeName;
		Collider* collider;
	};

	struct ModelData // 각 모델이 가지는 데이터..
	{
		map<int, vector<int>> preprocessedNodes; // 각 노드가 가지고 있는 자식노드들 인덱스
		map<int, bool> nodeCheck; // TreeNodeRecurs 방문흔적
		map<int, bool> createdCollidersCheck; // 해당인덱스노드가 컬라이더 가지고있는지 체크만.
		map<int, TreeNodeData> nodeCollidersMap;
		map<int, string> nodeNameMap;
		map<int, char[100]> colliderNameMap;

		string meshTextOnInspector;
		string materialTextOnInspector;
	};

	

	ColliderSettingScene();
	~ColliderSettingScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;


private:
	void selectModel();
	void showCreateModelButton();
	void InitializeModelDatas();

	void showModelHierarchyWindow();
	void showModelSelectWindow();
	void showColliderEditorWindow();
	void showAssetsWindow();
	void showModelInspector();
	void showTestWindow();

	void treeNodePreProcessing();
	void treeNodeRecurs(int nodesIndex);

	void exportFBX(string selectedFilePath, string fileNameToCreate, string parentFolderName);
	void playAssetsWindowDropEvent();
		
	void copyDraggedFile();
	void saveAsBinary();
	void saveAsCSV();
	void allSaveAsBinary();
	void allSaveAsCSV();
	//void loadBinaryFile();

	void loadFileList(string folderName, vector<string>& fileList);
	

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
	
	Monster* mMonster;
	Terrain* terrain;
	
	bool mbIsDropped;
	int mSelectedIndexFromAssets = -1;

	float mCurrentClipSpeed;
	float mCurrentClipTakeTime;

	// ColorPicking
	DepthStencil* mDepthStencil;
	RenderTarget* mRenderTargets[1];
	RenderTarget* mRenderTarget;

	Vector3 mMouseScreenPosition;
	ComputeShader* mComputeShader;
	ComputeStructuredBuffer* mComputeStructuredBuffer;
	ColorPickingInputBuffer* mInputBuffer;
	ColorPickingOutputBuffer* mOutputBuffer;

	Cube* mCube;

};