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
	void createMesh();
	void moveWorldCamera();


private:
	ModelObject* mBunny;
	ModelObject* mPlane;
	ModelAnimObject* mGroot;
	Sphere* mSphere;
	Camera* mWorldCamera;

	GBuffer* mGBuffer;

	Material* mMaterial;
	VertexBuffer* mVertexBuffer;
	Vector3 mPreFrameMousePosition;
};