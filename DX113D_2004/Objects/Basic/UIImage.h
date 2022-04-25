#pragma once

class UIImage : public Transform
{
public:
	UIImage(wstring shaderFile);
	virtual ~UIImage();

	virtual void Update();
	virtual void Render();

	void SetSRV(ID3D11ShaderResourceView* srv) { this->mSRV = srv; }

private:
	void CreateMesh();
	void CreateVP();

protected:
	Material* mMaterial;
	Mesh* mMesh;

	ID3D11ShaderResourceView* mSRV;

	Matrix mViewMatrix;
	Matrix mOrthographicMatrix;

	ViewBuffer* mViewBuffer;
	ProjectionBuffer* mProjectionBuffer;

	BlendState* mBlendStates[2];
	DepthStencilState* mDepthMode[2];
};