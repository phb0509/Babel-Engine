#pragma once

class Cube;

class Environment : public Singleton<Environment>
{
	friend class Singleton;

public:
	void PostRender();
	void Set(); // �����ʿ��ϱ��ѵ� �ļ���.
	void SetViewport(UINT width = WIN_WIDTH, UINT height = WIN_HEIGHT); // �ϴ� Environment���� ����. ������θ� Camera���� ���� ����.
	
	LightBuffer* GetLight() { return mLightBuffer; } 
	Vector3 GetLightPosition();

private:
	Environment();
	~Environment();

	void showLightInformation();

private:
	LightBuffer* mLightBuffer;
	D3D11_VIEWPORT mViewport;
	SamplerState* mSamplerState;
	Cube* mSun;
};