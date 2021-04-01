#pragma once

class UIImage : public Transform
{
protected:
	Material* material;
	Mesh* mesh;

	ID3D11ShaderResourceView* srv;

	Matrix view;
	Matrix orthographic;

	MatrixBuffer* viewBuffer;
	MatrixBuffer* projectionBuffer;

	BlendState* blendState[2];
	DepthStencilState* depthMode[2];
public:
	UIImage(wstring shaderFile);
	virtual ~UIImage();

	virtual void Update();
	virtual void Render();

	void SetSRV(ID3D11ShaderResourceView* srv) { this->srv = srv; }

private:
	void CreateMesh();
	void CreateVP();
};