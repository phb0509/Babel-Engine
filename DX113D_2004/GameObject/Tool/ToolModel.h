#pragma once

class ToolModel : public ModelAnimator, public Transform
{
public:
	ToolModel(string file);
	~ToolModel();

	void Update();
	void Render();
	void PostRender();
	void SetAnimation(int index);



private:
	int clipIndex;
	

public:
	vector<string> clips;
	int currentClipIndex;
};