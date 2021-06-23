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

	
public:
	TerrainEditor(UINT width = 500, UINT height = 500);
	~TerrainEditor();

	void Update();
	void Render();
	void PostRender();
		


private:
	void createMesh();
	void createNormal();
	void createTangent();
	void createCompute();

	bool computePicking(OUT Vector3* position);

	void adjustY(Vector3 position);
	void paintBrush(Vector3 position);
	bool checkMouseMove();

	void save(wstring heightFile);
	void load(wstring heightFile);


private:
	const float MAX_HEIGHT = 30.0f;

	Material* mMaterial;
	Mesh* mMesh;

	vector<VertexType> mVertices;
	vector<UINT> mIndices;

	UINT mWidth, mHeight;

	Texture* mHeightMap;
	Texture* mAlphaMap;
	Texture* mSecondMap;
	Texture* mThirdMap;

	ComputeShader* mComputeShader;
	RayBuffer* mRayBuffer;
	StructuredBuffer* mStructuredBuffer;
	InputDesc* mInput;
	OutputDesc* mOutput; // GPU에서 읽어온 값

	UINT mPolygonCount;

	BrushBuffer* mBrushBuffer;

	bool mbIsRaise;
	float mAdjustValue;

	bool mbIsPainting;
	float mPaintValue;

	int mSelectedMap;

	char mInputFileName[100];

	Vector3 mPickedPosition;
	Vector3 mCurrentMousePosition;
	Vector3 mLastPickingMousePosition;
};