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

	

	// Scene을(를) 통해 상속됨
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
	ToolModel* model;
	ToolModel* currentModel;

	string projectPath;
	string path;
	string currentModelName;

	vector<string> clips;
	vector<string> modelList;

	const char* clipTypes[20];
	const char* modelTypes[20];


	int currentModelIndex;
	int oldModelIndex;
	int currentClipIndex;
	int currentColliderIndex;

	map<string, vector<string>> clipsMap;
	vector<ToolModel*> models;
	vector<NodeData*> nodes;
	vector<Collider*> nodeColliders;

	map<int, vector<int>> preprocessedNodes;
	map<int, bool> nodeCheck;
	map<int, bool> createdCollidersCheck;
	map<int, Collider*> nodeCollidersMap;
	map<int, string> nodeNamesMap;
	map<int, char[100]> colliderTagMap;

	Collider* nodeCollider;



};