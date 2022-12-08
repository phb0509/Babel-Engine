#pragma once

class Cube;

class Environment : public Singleton<Environment>
{
	friend class Singleton;

public:
	void Set();
	void SetViewport(UINT width = WIN_WIDTH, UINT height = WIN_HEIGHT);
	
private:
	Environment();
	~Environment();

private:
	D3D11_VIEWPORT mViewport;
	SamplerState* mSamplerState;
};