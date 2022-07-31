#include "Framework.h"

UIImage::UIImage(wstring shaderFile):
	mWidthRatio(1.0f),
	mHeightRatio(1.0f)
{
	mMaterial = new Material(shaderFile);
	mTextureBuffer = new TextureBuffer();

	createMesh();	
	createViewBuffer();

	mBlendStates.push_back(new BlendState());
	mBlendStates.push_back(new BlendState());
	mBlendStates[1]->Alpha(false);

	mDepthModes.push_back(new DepthStencilState());
	mDepthModes.push_back(new DepthStencilState());
	mDepthModes[1]->DepthEnable(false);
}

UIImage::~UIImage()
{
	GM->SafeDelete(mMaterial);
	GM->SafeDelete(mMesh);
	GM->SafeDelete(mViewBuffer);
	GM->SafeDelete(mProjectionBuffer);
	GM->SafeDeleteVector(mBlendStates);
	GM->SafeDeleteVector(mDepthModes);
}

void UIImage::Update()
{
	mTextureBuffer->data.widthRatio = mWidthRatio;
	mTextureBuffer->data.heightRatio = mHeightRatio;
}

void UIImage::Render() // PostRender���ٰ� �Ѵܸ�����
{
	UpdateWorld();
	mMesh->SetIA();
	SetWorldBuffer(); // 0����
	mViewBuffer->SetVSBuffer(1);
	mProjectionBuffer->SetVSBuffer(2);

	DEVICECONTEXT->PSSetShaderResources(0, 1, &mSRV); // PS 0���� SRV�ѱ��.
	mTextureBuffer->SetPSBuffer(10);

	mMaterial->Set();

	//mBlendStates[1]->SetState();
	//mDepthModes[1]->SetState();
	DEVICECONTEXT->DrawIndexed(6, 0, 0);
	//mBlendStates[0]->SetState(); // �ٽ� 0������ SetState�ϴ� ���� => 1�� SetState�ϰ� �ٽ� �ȹٲ������ �ٸ��ſ� ������ ��ġ�� ����.
	//mDepthModes[0]->SetState();
}

void UIImage::createMesh()
{
	VertexUV vertices[4];
	vertices[0].position = { -0.5f, -0.5f, 0.0f };
	vertices[1].position = { -0.5f, +0.5f, 0.0f };
	vertices[2].position = { +0.5f, -0.5f, 0.0f };
	vertices[3].position = { +0.5f, +0.5f, 0.0f };

	vertices[0].uv = { 0, 1 };
	vertices[1].uv = { 0, 0 };
	vertices[2].uv = { 1, 1 };
	vertices[3].uv = { 1, 0 };

	UINT indices[] = {
		0, 1, 2,
		2, 1, 3
	};

	mMesh = new Mesh(vertices, sizeof(VertexUV), 4, indices, 6);
}

void UIImage::createViewBuffer()
{
	mViewMatrix = XMMatrixIdentity();
	mOrthographicMatrix = XMMatrixOrthographicOffCenterLH(0, WIN_WIDTH, 0, WIN_HEIGHT, -1, 1);

	mViewBuffer = new ViewBuffer();
	mViewBuffer->SetMatrix(mViewMatrix);

	mProjectionBuffer = new ProjectionBuffer();
	mProjectionBuffer->SetMatrix(mOrthographicMatrix);
}
