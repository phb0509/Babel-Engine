#pragma once

class ModelAnimObject : public Transform, public ModelAnimator
{
public:
	ModelAnimObject();
	~ModelAnimObject();

	void Update();
	void Render();
};