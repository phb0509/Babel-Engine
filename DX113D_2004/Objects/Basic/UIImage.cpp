#include "Framework.h"

UIImage::UIImage(wstring shaderFile):
	mWidthRatio(1.0f),
	mHeightRatio(1.0f),
	mbIsUsedUI(true),
	mbIsTargetMode(false)
{
	mMaterial = new Material(shaderFile);
	mTextureBuffer = new TextureBuffer();

	createMesh();	
	createTargetBuffer();

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
	GM->SafeDelete(mTargetViewBuffer);
	GM->SafeDelete(mTargetProjectionBuffer);
	GM->SafeDeleteVector(mBlendStates);
	GM->SafeDeleteVector(mDepthModes);
}

void UIImage::Update()
{
	mTextureBuffer->data.widthRatio = mWidthRatio;
	mTextureBuffer->data.heightRatio = mHeightRatio;
}

void UIImage::Render() 
{
	UpdateWorld();
	mMesh->SetIA();
	SetWorldBuffer(); // 0번에
	DEVICECONTEXT->PSSetShaderResources(0, 1, &mSRV);
	mTextureBuffer->SetPSBuffer(10);

	mMaterial->Set();

	if (!mbIsTargetMode) // 메인화면 UI.
	{
		mTargetViewBuffer->SetVSBuffer(1);
		mTargetProjectionBuffer->SetVSBuffer(2);
	}
	else // 몬스터상태 UI같은 타겟팅형 UI.
	{
		if (mCamera != nullptr)
		{
			mCamera->GetViewBuffer()->SetVSBuffer(1);
			mCamera->GetProjectionBufferInUse()->SetVSBuffer(2);
		}
	}

	if (mbIsUsedUI)
	{
		mBlendStates[0]->SetState();
		mDepthModes[1]->SetState();
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

	UINT indices[] = 
	{
		0, 1, 2,
		2, 1, 3
	};

	mMesh = new Mesh(vertices, sizeof(VertexUV), 4, indices, 6);
}

void UIImage::createTargetBuffer()
{
	Matrix targetViewMatrix = XMMatrixIdentity();
	Matrix targetProjectionMatrix = XMMatrixOrthographicOffCenterLH(0, WIN_WIDTH, 0, WIN_HEIGHT, -1, 1);

	mTargetViewBuffer = new ViewBuffer();
	mTargetViewBuffer->SetMatrix(targetViewMatrix);

	mTargetProjectionBuffer = new ProjectionBuffer();
	mTargetProjectionBuffer->SetMatrix(targetProjectionMatrix);
}
