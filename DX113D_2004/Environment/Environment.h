#pragma once

class Cube;

class Environment : public Singleton<Environment>
{
	friend class Singleton;

public:
	void PostRender();
	void Set(); // 수정필요하긴한데 후순위.
	void SetViewport(UINT width = WIN_WIDTH, UINT height = WIN_HEIGHT); // 일단 Environment에서 관리. 정석대로면 Camera에서 각각 보유.
	
private:
	Environment();
	~Environment();

	void showLightInformation();

private:
	D3D11_VIEWPORT mViewport;
	SamplerState* mSamplerState;
	BlendState* mBlendState[2];
	DepthStencilState* mDepthState[2];

};