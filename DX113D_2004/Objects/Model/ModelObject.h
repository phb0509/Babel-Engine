#pragma once

class ModelObject : public Transform
{
private:
	Model* model;
	Collider* collider;
public:
	ModelObject(string file, Collider::Type type);
	~ModelObject();

	void Render();

	Collider* GetCollider() { return collider; }
	Model* GetModel() { return model; }
};