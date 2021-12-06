#pragma once

typedef VertexUVNormalTangentBlend ModelVertex;

struct MaterialData
{
	string name;

	Float4 ambient;
	Float4 diffuse;
	Float4 specular;
	Float4 emissive;

	string diffuseMap;
	string specularMap;
	string normalMap;
};

struct MeshData
{
	string name;
	string materialName;

	vector<ModelVertex> vertices;
	vector<UINT> indices;
};

struct NodeData
{
	int index;
	string name;
	int parent;
	Float4x4 transform;
};

struct BoneData
{
	string name;
	int index;
	Float4x4 offset;
};

struct VertexWeights
{
	UINT indices[4];
	float weights[4];

	VertexWeights() : indices{}, weights{}
	{}

	void Add(const UINT& index, const float& weight) // weight는 본에대한 보간값.
	{
		for (UINT i = 0; i < 4; i++)
		{
			if (weights[i] == 0.0f) // 비어있으면
			{
				indices[i] = index;
				weights[i] = weight;
				return;
			}
		}
	}

	void Normalize()
	{
		float sum = 0.0f;

		for (UINT i = 0; i < 4; i++)
			sum += weights[i];

		for (UINT i = 0; i < 4; i++)
			weights[i] /= sum;
	}
};

struct KeyTransform // 키프레임 데이터구성 (SRT)
{
	float time; // 정확히 모르겠음 왜있는지

	// 해당 본이 어느정도 움직일지에 대한 SRT
	Float3 scale;
	Float4 rotation;
	Float3 position;
};

struct KeyFrame
{
	// 본 하나당 프레임 개수만큼의 키프레임 데이터를 가짐.
	string boneName;
	vector<KeyTransform> transforms; // time(frameCount), SRT값. 각 프레임마다 SRT값이 바뀌니까
};

struct ClipNode 
{
	vector<KeyTransform> keyFrame;
	aiString name;
};

struct Clip // 각각의 애니메이션. // 키프레임 정보를 가진다.
{
	string name;

	UINT frameCount; // 총 프레임개수. 
	float tickPerSecond; // 초당 프레임개수. 적으면 애니메이션이 부자연스럽고 많으면 데이터크기가 너무크고..
	float duration; // 애니메이션 총 시간. 클립의 길이.

	vector<KeyFrame*> keyFrame; // 키프레임 정보.
};

struct ClipTransform
{
	Matrix** transform;

	ClipTransform()
	{
		transform = new Matrix * [MAX_FRAME_KEY]; // 600

		for (UINT i = 0; i < MAX_FRAME_KEY; i++) // 행 : 프레임 열 : 본(노드)
			transform[i] = new Matrix[MAX_BONE];
	}

	~ClipTransform()
	{
		for (UINT i = 0; i < MAX_FRAME_KEY; i++)
			delete[] transform[i];

		delete[] transform;
	}
};