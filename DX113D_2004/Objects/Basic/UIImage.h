#pragma once

class UIImage : public Transform
{
public:
	UIImage(wstring shaderFile);
	virtual ~UIImage();

	virtual void Update();
	virtual void Render();
	void RenderTargetUI();
	
	void SetSRV(ID3D11ShaderResourceView* srv) { this->mSRV = srv; }
	void SetWidthRatio(float widthRatio) { mWidthRatio = widthRatio; }
	void SetHeightRatio(float heightRatio) { mHeightRatio = heightRatio; }
	void SetIsUsedUI(bool isUsed) { mbIsUsedUI = isUsed; }
	void SetCurMainCamera(Camera* mainCamera) { mCurMainCamera = mainCamera; }

private:
	void createMesh();
	void createMainUIBuffer();
	void createTargetUIBuffer();

protected:
	Material* mMaterial;
	Mesh* mMesh;
	ID3D11ShaderResourceView* mSRV;

	Camera* mCurMainCamera;

	ViewBuffer* mMainViewBuffer;
	ProjectionBuffer* mMainProjectionBuffer;

	ViewBuffer* mTargetViewBuffer;
	ProjectionBuffer* mTargetProjectionBuffer;

	TextureBuffer* mTextureBuffer;
	vector<BlendState*> mBlendStates;
	vector<DepthStencilState*> mDepthModes;

	float mWidthRatio;
	float mHeightRatio;

	bool mbIsUsedUI;
};