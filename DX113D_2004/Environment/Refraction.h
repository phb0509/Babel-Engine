#pragma once

class Refraction
{
public:
	Refraction(wstring normalFile);
	~Refraction();

	void Update();
	void PreRender();
	void Render();
	void PostRender();

private:
	TimeBuffer* mTimeBuffer;

	Texture* mNormalMap;

	RenderTarget* mRenderTarget;
	DepthStencil* mDepthStencil;

	class UIImage* mTargetTexture;
};