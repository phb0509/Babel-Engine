#pragma once

class DeferredRenderingScene : public Scene
{

public:
	DeferredRenderingScene();
	~DeferredRenderingScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;

private:
	void moveWorldCamera();

private:
	ModelAnimObject* mPlayer;
	ModelObject* mBunny;
	ModelObject* mPlane;
	ModelAnimObject* mGroot;
	Sphere* mSphere;
	Camera* mWorldCamera;

	GBuffer* mGBuffer;

	Material* mDeferredMaterial;
	VertexBuffer* mVertexBuffer;
	Vector3 mPreFrameMousePosition;
	LightBuffer* mLightBuffer;
	Terrain* mTerrain;

	Light* mDirectionalLight;
	Light* mPointLight;
	Light* mSpotLight;
	Light* mCapsuleLight;

};