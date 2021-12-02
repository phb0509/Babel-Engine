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

	/*void SetHasTPoseClip(bool hasTPoseClip) { mbHasTPoseClip = hasTPoseClip; }
	bool GetHasTPoseClip() { return mbHasTPoseClip; }*/

	void SetName(string name) { mName = name; }
	string GetName() { return mName; }

private:
	int mClipIndex;
	string mName;

public:
	vector<string> mClipNames;
	int mCurrentClipIndex;
	bool mbIsSkinnedMesh;
	//bool mbHasTPoseClip;
};