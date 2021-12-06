#pragma once

class ToolModel : public ModelAnimator, public Transform
{

public:
	ToolModel(string name);
	~ToolModel();

	void Update();
	void Render();
	void PostRender();
	void SetAnimation(int index);

	void SetIsSkinnedMesh(bool isSkinnedMesh) { mbIsSkinnedMesh = isSkinnedMesh; }
	bool GetIsSkinnedMesh() { return mbIsSkinnedMesh; }

	int GetCurrentClipIndex() { return mCurrentClipIndex; }

	void SetName(string name) { mName = name; }
	string GetName() { return mName; }

	void SetClip(string modelName, string clipfileName);

private:
	int mClipIndex;
	string mName;
	vector<string> mClipNames;
	int mCurrentClipIndex;
	bool mbIsSkinnedMesh;
	
};