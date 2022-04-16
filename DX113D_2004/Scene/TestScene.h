#pragma once

class TestScene : public Scene
{
private:

public:
	TestScene();
	~TestScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;


private:
	void moveWorldCamera();

private:

	Terrain* mTerrain;
	Camera* mWorldCamera;
	Vector3 mPreFrameMousePosition;
	GBuffer* mGBuffer;
	Material* mMaterial;
	VertexBuffer* mVertexBuffer;

	InstancingMutants* mInstancingMutants;
	vector<Monster*> mInstanceMutants;
	int mMutantInstanceCount;
	bool mbIsInstancingMode;

	vector<RenderTarget*> mRenderTargets;
	DepthStencil* mDepthStencil;
	Sphere* mSphere;
};