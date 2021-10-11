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

	
	struct OutputUVDesc
	{
		Float3 worldPosition;
		float padding1;
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

	struct AddedTextureInfo
	{
		Texture* texture;
		wstring textureName;

		AddedTextureInfo(Texture* _texture, wstring _textureName) : texture(_texture), textureName(_textureName) {}
	};

public:
	TerrainEditor(UINT width = 500, UINT height = 500);
	~TerrainEditor();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
		


private:
	void createMesh();
	void createNormal();
	void createTangent();
	void createCompute();
	void createPixelPickingCompute();
	void createTestCompute();
	

	bool computePicking(OUT Vector3* position);
	void computePixelPicking(OUT Vector3* position);
	void computeTestPicking();

	void adjustY(Vector3 position);
	void paintBrush(Vector3 position);
	bool checkMouseMove();

	void saveHeightMap(wstring heightFileName);
	void saveTextureMap(wstring textureFileName);
	void loadHeightMap(wstring heightFile);

	void changeHeightMap(wstring heightFileName);
	void changeTextureMap(wstring textureFileName);

	void addTexture();
	void showAddedTextures();
	void getFileNames(string path);
	void showTextureAsset();
	


private:
	const float MAX_HEIGHT = 30.0f;

	Material* mMaterial;
	Mesh* mMesh;

	vector<VertexType> mVertices;
	vector<UINT> mIndices;

	UINT mWidth, mHeight;

	Texture* mHeightMap;

	ComputeShader* mComputeShader;
	RayBuffer* mRayBuffer;
	ComputeStructuredBuffer* mComputePickingStructuredBuffer;
	InputDesc* mInput;
	OutputDesc* mOutput; // GPU에서 읽어온 값
	UINT mPolygonCount;

	BrushBuffer* mBrushBuffer;

	float mAdjustValue;

	int mbIsPainting;
	float mPaintValue;

	int mPickedLayerIndex;

	char mInputFileName[100];

	Vector3 mPickedPosition;
	Vector3 mCurrentMousePosition;
	Vector3 mLastPickingMousePosition;

	wstring mCurrentHeightMapFileName;
	wstring mCurrentTextureMapFileName;

	vector<AddedTextureInfo> mAddedTextures;
	int mCurrentTextureIndex;

	Texture* mTerrainDiffuseMap;
	vector<const char*> mLayerNames;
	vector<Texture*> mLayers; // 셰이더에 넘길 레이어들. 

	ID3D11ShaderResourceView* mLayerSRVs[4];
	vector<string> mTextureAssetFileNames;

	Vector3 testPos;
	RenderTarget* mRenderTargets[1];
	ID3D11ShaderResourceView* mRenderTargetSRVs[2];
	DepthStencil* mDepthStencil;
	RenderTarget* mDepthRenderTarget;
	Vector3 mMouseScreenPosition;
	Vector3 mMouseNDCPosition;

	VertexShader* mDepthVertexShader;
	PixelShader* mDepthPixelShader;

	UIImage* mRenderTextures[2];
	Texture* mTempTexture;
	Material* mDepthMaterial;
	
	Texture2DBuffer* mTexture2DBuffer;

	UINT tWidth;
	UINT tHeight;

	bool mbIsUVPicking = false;

	ComputeStructuredBuffer* mPixelPickingStructuredBuffer;
	OutputUVDesc* mOutputUVDesc;
	MouseUVBuffer* mMouseUVBuffer;
	OutputUVDesc mTestOutputDesc;
	float mDepthRedValue = 0.0f;

	ID3D11ShaderResourceView* mLayerArray[4];
};