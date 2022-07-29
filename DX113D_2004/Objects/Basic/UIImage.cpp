#include "Framework.h"

UIImage::UIImage(wstring shaderFile):
	mWidthRatio(1.0f),
	mHeightRatio(1.0f)
{
	mMaterial = new Material(shaderFile);
	createMesh();	
	createViewBuffer();

	mBlendStates[0] = new BlendState();
	mBlendStates[1] = new BlendState();
	mBlendStates[1]->Alpha(false);

	mDepthMode[0] = new DepthStencilState();
	mDepthMode[1] = new DepthStencilState();
	mDepthMode[1]->DepthEnable(false);
}

UIImage::~UIImage()
{
	delete mMaterial;
	delete mMesh;

	delete mViewBuffer;
	delete mProjectionBuffer;

	delete mBlendStates[0];
	delete mBlendStates[1];

	delete mDepthMode[0];
	delete mDepthMode[1];
}

void UIImage::Update()
{
	
}

void UIImage::Render() // PostRender에다가 한단말이지
{
	UpdateWorld();
	mMesh->SetIA();
	SetWorldBuffer(); // 0번에
	mViewBuffer->SetVSBuffer(1);
	mProjectionBuffer->SetVSBuffer(2);

	DEVICECONTEXT->PSSetShaderResources(0, 1, &mSRV); // PS 0번에 SRV넘기기.

	mMaterial->Set();

	mBlendStates[1]->SetState();
	mDepthMode[1]->SetState();
	DEVICECONTEXT->DrawIndexed(6, 0, 0);
	mBlendStates[0]->SetState(); // 다시 0번으로 SetState하는 이유 => 1번 SetState하고 다시 안바꿔놓으면 다른거에 영향을 미치기 떄문.
	mDepthMode[0]->SetState();
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
