#pragma once

class ModelReader
{

public:
	ModelReader();
	virtual ~ModelReader();

	void MeshRender();
	void MeshRender(UINT drawCount);

	void ReadMaterial(string folderName,string fileName);
	void ReadMesh(string folderName,string fileName);

	void SetShader(wstring file);
	void SetMesh(string folderName, string fileName);
	void SetMaterial(string folderName, string fileName);
	void SetDiffuseMap(wstring file);
	void SetSpecularMap(wstring file);
	void SetNormalMap(wstring file);
	void IASet();

	int GetNodeIndex(string name);
	bool GetHasMeshes();

	void SetBox(Vector3* minBox, Vector3* maxBox);
	vector<NodeData*> GetNodes() {return mNodes;}
	vector<ModelMesh*> GetMeshes() { return mMeshes; }

private:
	void deleteDatas();

protected:
	map<string, Material*> mMaterials;
	vector<ModelMesh*> mMeshes;
	vector<NodeData*> mNodes;
	vector<BoneData*> mBones;

	map<string, UINT> mBoneMap;

	TypeBuffer* mTypeBuffer;
	Material* mDefaultMaterial;

	bool mbIsSetColorPickingShader;

};