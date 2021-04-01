#pragma once

class Transform
{
public:
	static bool isAxisDraw;

	bool isActive;

	string tag;

	Vector3 position;
	Vector3 rotation;
	Vector3 scale;	
protected:
	Vector3 globalPosition;
	Vector3 globalRotation;
	Vector3 globalScale;

	Vector3 pivot;

	Matrix world;
	Matrix* parent;

	MatrixBuffer* worldBuffer;

private:
	Material* material;
	Mesh* mesh;

	MatrixBuffer* transformBuffer;

	vector<VertexColor> vertices;
	vector<UINT> indices;
public:
	Transform(string tag = "Untagged");
	virtual ~Transform();

	void UpdateWorld();
	void RenderAxis();

	void SetWorldBuffer(UINT slot = 0);

	Matrix* GetWorld() { return &world; }
	void SetParent(Matrix* value) { parent = value; }

	Vector3 Forward();
	Vector3 Up();
	Vector3 Right();

	Vector3 GlobalPos() { return globalPosition; }
	Vector3 GlobalRot() { return globalRotation; }
	Vector3 GlobalScale() { return globalScale; }

private:
	void CreateAxis();
};