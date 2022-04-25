#pragma once

class Shadow
{
public:
	Shadow(UINT width = 2000, UINT height = 2000);
	~Shadow();

	void PreRender();
	void Render();
	void PostRender();

	void SetViewProjection();
	void SetLight(Light* light) { mLight = light; }

private:
	UINT mWidth;
	UINT mHeight;
	float mRadius;

	RenderTarget* mRenderTarget;
	DepthStencil* mDepthStencil;

	class UIImage* mDepthMap;

	MatrixBuffer* mViewBuffer;
	MatrixBuffer* mProjectionBuffer;

	TypeBuffer* mQualityBuffer;
	SizeBuffer* mSizeBuffer;
	Light* mLight;
};