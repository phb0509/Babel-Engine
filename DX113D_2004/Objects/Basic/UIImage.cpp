#include "Framework.h"

UIImage::UIImage(wstring shaderFile) :
	mWidthRatio(1.0f),
	mHeightRatio(1.0f),
	mbIsUsedUI(true)
{
	mMaterial = new Material(shaderFile);
	mTextureBuffer = new TextureBuffer();

	createMesh();
	createMainUIBuffer();

	mBlendStates.push_back(new BlendState());
	mBlendStates.push_back(new BlendState());
	mBlendStates[0]->SetAlpha(true);
	mBlendStates[1]->SetAlpha(false);

	mDepthModes.push_back(new DepthStencilState());
	mDepthModes.push_back(new DepthStencilState());
	mDepthModes[0]->DepthEnable(true);
	mDepthModes[1]->DepthEnable(false);
}

UIImage::~UIImage()
{
	GM->SafeDelete(mMaterial);
	GM->SafeDelete(mMesh);
	GM->SafeDelete(mMainViewBuffer);
	GM->SafeDelete(mMainProjectionBuffer);
	GM->SafeDeleteVector(mBlendStates);
	GM->SafeDeleteVector(mDepthModes);
}

void UIImage::Update()
{
	if (mbIsUsedUI)
	{
		mTextureBuffer->data.widthRatio = mWidthRatio;
		mTextureBuffer->data.heightRatio = mHeightRatio;

		UpdateWorld();
	}
}

void UIImage::Render()
{
	mMesh->SetIA();

	SetWorldBuffer(); // 0번에
	mMainViewBuffer->SetVSBuffer(1);
	mMainProjectionBuffer->SetVSBuffer(2);
	DEVICECONTEXT->PSSetShaderResources(0, 1, &mSRV);
	mTextureBuffer->SetPSBuffer(6);

	mMaterial->Set();

	if (mbIsUsedUI) // 실제 인게임UI.
	{
		mBlendStates[0]->SetState(); // true Alpha
		mDepthModes[1]->SetState(); // false Depth
		DEVICECONTEXT->DrawIndexed(6, 0, 0);
		mBlendStates[1]->SetState();
		mDepthModes[0]->SetState();
	}
	else
	{
		mBlendStates[1]->SetState();
		mDepthModes[1]->SetState();
		DEVICECONTEXT->DrawIndexed(6, 0, 0);
		mBlendStates[1]->SetState();
		mDepthModes[0]->SetState();
	}

	ID3D11ShaderResourceView* nullSRV = NULL;
	DEVICECONTEXT->PSSetShaderResources(0, 1, &nullSRV);
}

void UIImage::RenderTargetUI()
{
	mMesh->SetIA();
	SetWorldBuffer(); // 0번에
	mMainViewBuffer->SetVSBuffer(1);
	mMainProjectionBuffer->SetVSBuffer(2);
	DEVICECONTEXT->PSSetShaderResources(0, 1, &mSRV);
	mTextureBuffer->SetPSBuffer(6);

	mMaterial->Set();

	mBlendStates[0]->SetState();
	mDepthModes[1]->SetState();
	DEVICECONTEXT->DrawIndexed(6, 0, 0);
	mBlendStates[1]->SetState();
	mDepthModes[0]->SetState();

	//else
	//{
	//	mBlendStates[1]->SetState();
	//	mDepthModes[1]->SetState();
	//	DEVICECONTEXT->DrawIndexed(6, 0, 0);
	//	mBlendStates[1]->SetState();
	//	mDepthModes[0]->SetState();
	//}

	ID3D11ShaderResourceView* nullSRV = NULL;
	DEVICECONTEXT->PSSetShaderResources(0, 1, &nullSRV);
}

void UIImage::createMesh()
{
	VertexUV vertices[4];
	//vertices[0].position = { -0.5f, -0.5f, 0.0f };
	//vertices[1].position = { -0.5f, +0.5f, 0.0f };
	//vertices[2].position = { +0.5f, -0.5f, 0.0f };
	//vertices[3].position = { +0.5f, +0.5f, 0.0f };

	vertices[0].position = { 0.0f,  0.0f, 0.0f };
	vertices[1].position = { 0.0f, +1.0f, 0.0f };
	vertices[2].position = { +1.0f,  0.0f, 0.0f };
	vertices[3].position = { +1.0f, +1.0f, 0.0f };

	vertices[0].uv = { 0, 1 };
	vertices[1].uv = { 0, 0 };
	vertices[2].uv = { 1, 1 };
	vertices[3].uv = { 1, 0 };

	UINT indices[] =
	{
		0, 1, 2,
		2, 1, 3
	};

	mMesh = new Mesh(vertices, sizeof(VertexUV), 4, indices, 6);
}

void UIImage::createMainUIBuffer()
{
	Matrix mainViewMatrix = XMMatrixIdentity();
	Matrix mainOrthographicProjectionMatrix = XMMatrixOrthographicOffCenterLH(0, WIN_WIDTH, 0, WIN_HEIGHT, -1, 1);

	mMainViewBuffer = new ViewBuffer();
	mMainViewBuffer->SetMatrix(mainViewMatrix);

	mMainProjectionBuffer = new ProjectionBuffer();
	mMainProjectionBuffer->SetMatrix(mainOrthographicProjectionMatrix);
}

void UIImage::createTargetUIBuffer()
{
	Matrix mainViewMatrix = XMMatrixIdentity();
	Matrix mainProjectionMatrix = XMMatrixOrthographicOffCenterLH(0, WIN_WIDTH, 0, WIN_HEIGHT, -1, 1);

	mMainViewBuffer = new ViewBuffer();
	mMainViewBuffer->SetMatrix(mainViewMatrix);

	mMainProjectionBuffer = new ProjectionBuffer();
	mMainProjectionBuffer->SetMatrix(mainProjectionMatrix);
}
