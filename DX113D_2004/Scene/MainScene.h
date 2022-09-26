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
	void printToCSV();
	void moveWorldCamera();
	void executeEvent();
	void respawnMonster();

private:
	Player* mPlayer;
	PlayerStatusBar* mPlayerStatusBar;
	Monster* mMonster;
	Terrain* mTerrain;
	vector<Monster*> mMutants;

	ModelObject* mObstacle1;
	ModelObject* mObstacle2;

	Frustum* mTargetCameraFrustum;
	vector<Matrix> boneMatrix;

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
	VertexBuffer* mVertexBuffer;

	RenderTarget* mDirectionalLightDepthMapForShow;
	DepthStencil* mDirectionalLightDSV;
	DirectionalLightBuffer* mDirectionalLightBuffer;
};