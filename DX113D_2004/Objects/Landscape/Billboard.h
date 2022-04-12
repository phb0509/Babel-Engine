#pragma once

class Billboard : public Transform
{
public:
	Billboard(wstring diffuseFile);
	~Billboard();

	void Update();
	void Render();
	void SetCamera(Camera* camera) { mCamera = camera; }
private:
	void CreateMesh();


public:
	Material* mMaterial;
	Mesh* mMesh;
	Camera* mCamera;
};