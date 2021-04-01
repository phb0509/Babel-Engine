#pragma once

class ModelReader
{
protected:
	map<string, Material*> materials;
	vector<ModelMesh*> meshes;
	vector<NodeData*> nodes;
	vector<BoneData*> bones;

	map<string, UINT> boneMap;

	TypeBuffer* typeBuffer;
public:
	ModelReader(string file);
	virtual ~ModelReader();

	void MeshRender();
	void MeshRender(UINT drawCount);

	void ReadMaterial(string file);
	void ReadMesh(string file);

	void SetShader(wstring file);

	void SetDiffuseMap(wstring file);
	void SetSpecularMap(wstring file);
	void SetNormalMap(wstring file);

	int GetNodeByName(string name);

	void SetBox(Vector3* minBox, Vector3* maxBox);
	vector<NodeData*> GetNodes() {return nodes;}
};