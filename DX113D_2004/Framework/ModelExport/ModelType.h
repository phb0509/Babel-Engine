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

	void Add(const UINT& index, const float& weight) // weight�� �������� ������.
	{
		for (UINT i = 0; i < 4; i++)
		{
			if (weights[i] == 0.0f) // ���������
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

struct KeyTransform // Ű������ �����ͱ��� (SRT)
{
	float time; // ��Ȯ�� �𸣰��� ���ִ���

	// �ش� ���� ������� ���������� ���� SRT
	Float3 scale;
	Float4 rotation;
	Float3 position;
};

struct KeyFrame
{
	// �� �ϳ��� ������ ������ŭ�� Ű������ �����͸� ����.
	string boneName;
	vector<KeyTransform> transforms; // time(frameCount), SRT��. �� �����Ӹ��� SRT���� �ٲ�ϱ�
};

struct ClipNode 
{
	vector<KeyTransform> keyFrame;
	aiString name;
};

struct Clip // ������ �ִϸ��̼�. // Ű������ ������ ������.
{
	string name;

	UINT frameCount; // �� �����Ӱ���. 
	float tickPerSecond; // �ʴ� �����Ӱ���. ������ �ִϸ��̼��� ���ڿ������� ������ ������ũ�Ⱑ �ʹ�ũ��..
	float duration; // �ִϸ��̼� �� �ð�. Ŭ���� ����.

	vector<KeyFrame*> keyFrame; // Ű������ ����.
};

struct ClipTransform
{
	Matrix** transform;

	ClipTransform()
	{
		transform = new Matrix * [MAX_FRAME_KEY]; // 600

		for (UINT i = 0; i < MAX_FRAME_KEY; i++) // �� : ������ �� : ��(���)
			transform[i] = new Matrix[MAX_BONE];
	}

	~ClipTransform()
	{
		for (UINT i = 0; i < MAX_FRAME_KEY; i++)
			delete[] transform[i];

		delete[] transform;
	}
};