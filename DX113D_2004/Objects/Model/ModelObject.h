#pragma once

class ModelObject : public Transform
{
public:
	ModelObject();
	~ModelObject();

	void Render();

	Collider* GetCollider() { return mCollider; }
	Model* GetModel() { return mModel; }

private:
	Model* mModel;
	Collider* mCollider;
};