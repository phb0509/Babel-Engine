#pragma once

class MainScene : public Scene
{
public:
	enum class eCamera
	{
		WorldCamera,
		TargetCamera
	};

	MainScene();
	~MainScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	
private:
	void updateLight();
	void moveLight();
	void updateCamera();
	void moveWorldCamera();
	void executeEvent();
	void respawnMonster();
	void deferredRender();

	//void sha

	void initDeferred();
	void initLight();
	void initCamera();
	void initTerrain();
	void initPlayer();
	void initInstances();
	void initShadowMapping();

	void showCameraInfo();
	void showShadowDepthMap();

	void renderShadowDepth();
	void renderGBuffer();

private:
	Player* mPlayer;
	Terrain* mTerrain;

	InstancingMutants* mInstancingMutants;
	vector<Monster*> mInstanceMutants;
	int mMutantInstanceCount;
	bool mbIsInstancingMode;

	Camera* mWorldCamera;
	Camera* mTargetCamera;
	Camera* mTargetCameraForShow;
	bool mbIsWorldMode;

	eCamera mMainCamera;
	Vector3 mPreFrameMousePosition;

	LightBuffer* mLightBuffer;
	Light* mDirectionalLight;

	GBuffer* mGBuffer;
	Material* mDeferredMaterial;
	VertexBuffer* mDeferredVertexBuffer;

	RenderTarget* mDirectionalLightDepthMapForShow;
	DepthStencil* mDirectionalLightDSV;
	ShadowMappingLightBuffer* mShadowMappingLightBuffer;

	UINT mDepthWidth;
	UINT mDepthHeight;
};