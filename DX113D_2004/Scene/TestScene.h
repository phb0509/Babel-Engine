#pragma once

class TestScene : public Scene
{
	struct InputDesc
	{

	};

	struct OutputDesc
	{
		
	};

public:
	TestScene();
	~TestScene();

	// Scene��(��) ���� ��ӵ�
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;


private:
	Texture2DBuffer* mTexture2DBuffer;
	ComputeShader* mComputeShader;
	Texture* mTempTexture;
	TextureClass* mTextureClass;
	ComputeStructuredBuffer* mStructuredBuffer;
	OutputDesc* mOutput;
};