#pragma once

class ModelAnimObject : public Transform, public ModelAnimator
{
public:
	ModelAnimObject(string file);
	~ModelAnimObject();

	void Update();
	void Render();
};