#pragma once

class ToolModel : public ModelAnimator, public Transform
{

public:
	ToolModel(string name);
	~ToolModel();

	void Update();
	void Render();
	void PostRender();
	void SetAnimation(int index, int speed = 1.0f, int takeTime = 0.2f);

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