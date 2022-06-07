#pragma once

class Reflection
{
public:
	Reflection(Transform* transform);
	~Reflection();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void SetTargetCamera(Camera* camera) { mTargetCamera = camera; }

private:
	Transform* mTransform;

	MatrixBuffer* mReflectionBuffer;

	RenderTarget* mRenderTarget;
	DepthStencil* mDepthStencil;

	Camera* mWorldCamera; 
	Camera* mTargetCamera; 
	class UIImage* mTargetTexture;
};