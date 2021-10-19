#pragma once

class ModelExporter
{
private:
	Assimp::Importer* importer;
	const aiScene* scene; // assimp에 있는 구조체.

	vector<MaterialData*> materials;
	vector<MeshData*> meshes;
	vector<NodeData*> nodes;
	vector<BoneData*> bones;

	map<string, UINT> boneMap;
	UINT boneCount;
public:
	ModelExporter(string file);
	~ModelExporter();

	void ExportMaterial(string savePath);
	void ExportMesh(string savePath);
	void ExportClip(string savePath);

	//Material///////////////////////////
	void ReadMaterial();
	void WriteMaterial(string savePath);
	string CreateTexture(string savePath, string file);
	
	//Mesh///////////////////////////////
	void ReadMesh(aiNode* node);
	void ReadNode(aiNode* node, int index, int parent);
	void ReadBone(aiMesh* mesh, vector<VertexWeights>& vertexWeights);
	void WriteMesh(string savePath);

	//Clip///////////////////////////////
	Clip* ReadClip(aiAnimation* animation);
	void ReadKeyFrame(Clip* clip, aiNode* node, vector<ClipNode>& clipNodes);
	void WriteClip(Clip* clip, string savePath);
};