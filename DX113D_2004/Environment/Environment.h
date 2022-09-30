#pragma once

class Cube;

class Environment : public Singleton<Environment>
{
	friend class Singleton;

public:
	void Set(); // �����ʿ��ϱ��ѵ� �ļ���.
	void SetViewport(UINT width = WIN_WIDTH, UINT height = WIN_HEIGHT); // �ϴ� Environment���� ����. ������θ� Camera���� ���� ����.
	
private:
	Environment();
	~Environment();

private:
	D3D11_VIEWPORT mViewport;
	SamplerState* mSamplerState;
};