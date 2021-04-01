#pragma once

class Billboard : public Transform
{
public:
	Material* material;
	Mesh* mesh;

public:
	Billboard(wstring diffuseFile);
	~Billboard();

	void Update();
	void Render();

private:
	void CreateMesh();
};