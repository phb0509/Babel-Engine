#pragma once

class TerrainEditor : public Transform
{
private:
	typedef VertexUVNormalTangentAlpha VertexType;

	class BrushBuffer : public ConstBuffer
	{
	public:
		struct Data
		{
			int type;
			Float3 location;

			float range;
			Float3 color;
		}data;

		BrushBuffer() : ConstBuffer(&data, sizeof(Data))
		{
			data.location = Float3(0, 0, 0);			
			data.color = Float3(0, 0.5f, 0);

			data.type = 0;
			data.range = 10.0f;
		}
	};

	struct InputDesc
	{
		UINT index;
		Float3 v0, v1, v2;
	};

	struct OutputDesc
	{
		int picked;
		float u, v, distance;
	};

	const float MAX_HEIGHT = 30.0f;

	Material* material;
	Mesh* mesh;

	vector<VertexType> vertices;
	vector<UINT> indices;

	UINT width, height;

	Texture* heightMap;
	Texture* alphaMap;
	Texture* secondMap;
	Texture* thirdMap;

	ComputeShader* computeShader;
	RayBuffer* rayBuffer;
	StructuredBuffer* structuredBuffer;
	InputDesc* input;
	OutputDesc* output;

	UINT size;

	BrushBuffer* brushBuffer;

	bool isRaise;
	float adjustValue;

	bool isPainting;
	float paintValue;

	int selectMap;

	char inputFileName[100];
public:
	TerrainEditor(UINT width = 100, UINT height = 100);
	~TerrainEditor();

	void Update();
	void Render();
	void PostRender();
		
	bool ComputePicking(OUT Vector3* position);

	void AdjustY(Vector3 position);
	void PaintBrush(Vector3 position);

	void Save(wstring heightFile);
	void Load(wstring heightFile);
private:
	void CreateMesh();
	void CreateNormal();
	void CreateTangent();
	void CreateCompute();
};