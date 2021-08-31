#include "Framework.h"
#include "TestScene.h"

TestScene::TestScene()
{
	mTempTexture = Texture::Add(L"Textures/Landscape/TestBlueImage.png");
	mComputeShader = Shader::AddCS(L"TestCS");
	
	int pixelCount = mTempTexture->GetWidth() * mTempTexture->GetHeight(); // 404952

	mStructuredBuffer = new ComputeStructuredBuffer(sizeof(OutputDesc), pixelCount); //mInput�� �ͷ��ι��ؽ����� ����ִ�.
		
	mOutput = new OutputDesc[pixelCount];
	
	mComputeShader->Set();
	DEVICECONTEXT->CSSetShaderResources(1, 1, &mTempTexture->GetSRV());
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mStructuredBuffer->GetUAV(), nullptr);

	UINT width = mTempTexture->GetWidth();
	UINT height = mTempTexture->GetHeight();
	
	//UINT x = ((float)width / 32.0f) < 1.0f ? 1.0f : ((float)width / 32.0f);
	//UINT y = ((float)height / 32.0f) < 1.0f ? 1.0f : ((float)height / 32.0f);

	//DEVICECONTEXT->Dispatch((UINT)ceilf(x), (UINT)ceilf(y), 1);

	UINT x = (UINT)ceilf((float)pixelCount / 1024.0f);
	DEVICECONTEXT->Dispatch(x, 1, 1);

	mStructuredBuffer->Copy(mOutput, sizeof(OutputDesc) * pixelCount); // GPU���� ����Ѱ� �޾ƿ�. // ���⼭ ������ ���̸���.

}

TestScene::~TestScene()
{
}

void TestScene::Update()
{

}

void TestScene::PreRender()
{
}

void TestScene::Render()
{
	/*mComputeShader->Set();

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mTexture2DBuffer->GetSRV());
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mTexture2DBuffer->GetUAV(), nullptr);

	UINT width = mTexture2DBuffer->GetWidth();
	UINT height = mTexture2DBuffer->GetHeight();
	UINT page = mTexture2DBuffer->GetPage();

	UINT x = ((float)width / 32.0f) < 1.0f ? 1.0f : ((float)width / 32.0f);
	UINT y = ((float)height / 32.0f) < 1.0f ? 1.0f : ((float)height / 32.0f);

	DEVICECONTEXT->Dispatch((UINT)ceilf(x), (UINT)ceilf(y), page);*/
}

void TestScene::PostRender()
{
	int frame_padding = 2;
	ImVec2 bigSize = ImVec2(128.0f, 128.0f);
	ImVec2 size = ImVec2(64.0f, 64.0f); // �̹�����ư ũ�⼳��.                     
	ImVec2 uv0 = ImVec2(0.0f, 0.0f); // ������̹��� uv��ǥ����.
	ImVec2 uv1 = ImVec2(1.0f, 1.0f); // ��ü�� ����ҰŴϱ� 1.
	ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // ������.(Background Color)        
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	ImGui::ImageButton(mTexture2DBuffer->OutputSRV(), bigSize, uv0, uv1, frame_padding, bg_col, tint_col);
	//ImGui::ImageButton(mTempTexture->GetTestSRV(), bigSize, uv0, uv1, frame_padding, bg_col, tint_col);
	//ImGui::ImageButton(mTexture2DBuffer->GetSRV(), bigSize, uv0, uv1, frame_padding, bg_col, tint_col);
	
}
