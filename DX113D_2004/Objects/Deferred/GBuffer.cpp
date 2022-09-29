#include "Framework.h"

GBuffer::GBuffer()
{
	mDiffuseRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	mSpecularRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	mNormalRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	mDepthRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	mSpecularRenderTargetForShow = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);

	mRenderTargets.push_back(mDiffuseRenderTarget);
	mRenderTargets.push_back(mSpecularRenderTarget);
	mRenderTargets.push_back(mNormalRenderTarget);
	mRenderTargets.push_back(mDepthRenderTarget);
	mRenderTargets.push_back(mSpecularRenderTargetForShow);

	mDepthStencil = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true);

	mSRVs[0] = mDepthStencil->GetSRV(); // ����Ÿ���� SRV.
	mSRVs[1] = mDiffuseRenderTarget->GetSRV();
	mSRVs[2] = mSpecularRenderTarget->GetSRV();
	mSRVs[3] = mNormalRenderTarget->GetSRV();

	mShowSRVs[0] = mDepthRenderTarget->GetSRV(); // ����Ÿ���� SRV.
	mShowSRVs[1] = mDiffuseRenderTarget->GetSRV();
	mShowSRVs[2] = mSpecularRenderTargetForShow->GetSRV();
	mShowSRVs[3] = mNormalRenderTarget->GetSRV();
}

GBuffer::~GBuffer()
{
	GM->SafeDelete(mDiffuseRenderTarget);
	GM->SafeDelete(mSpecularRenderTarget);
	GM->SafeDelete(mNormalRenderTarget);
	GM->SafeDelete(mDepthStencil);
}

void GBuffer::PreRender()
{
	RenderTarget::ClearAndSetWithDSV(mRenderTargets.data(), 5, mDepthStencil); // RTV�迭,RTV�迭 ������(����), depthStencil // OM�� SetRenderTarget	   
}

void GBuffer::SetTexturesToPS()
{
	DEVICECONTEXT->PSSetShaderResources(10, 1, &mSRVs[0]); // ����
	DEVICECONTEXT->PSSetShaderResources(11, 1, &mSRVs[1]); // ��ǻ��
	DEVICECONTEXT->PSSetShaderResources(12, 1, &mSRVs[2]); // ����ŧ��
	DEVICECONTEXT->PSSetShaderResources(13, 1, &mSRVs[3]); // �븻
}

void GBuffer::PostRender()
{
	ImGui::Begin("DeferredTextures");

	int frame_padding = 0;
	ImVec2 imageButtonSize = ImVec2(200.0f, 200.0f); // �̹�����ư ũ�⼳��.                     
	ImVec2 imageButtonUV0 = ImVec2(0.0f, 0.0f); // ������̹��� uv��ǥ����.
	ImVec2 imageButtonUV1 = ImVec2(1.0f, 1.0f); // ��ü�� ����ҰŴϱ� 1.
	ImVec4 imageButtonBackGroundColor = ImVec4(0.06f, 0.06f, 0.06f, 0.94f); // ImGuiWindowBackGroundColor.
	ImVec4 imageButtonTintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	ImGui::ImageButton(mShowSRVs[0], imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);
	ImGui::SameLine();
	ImGui::ImageButton(mShowSRVs[1], imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);

	ImGui::ImageButton(mShowSRVs[2], imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);
	ImGui::SameLine();
	ImGui::ImageButton(mShowSRVs[3], imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);

	ImGui::End();
}

void GBuffer::ClearSRVs()
{
	ID3D11ShaderResourceView* pSRV = NULL;

	DEVICECONTEXT->PSSetShaderResources(10, 1, &pSRV);
	DEVICECONTEXT->PSSetShaderResources(11, 1, &pSRV);
	DEVICECONTEXT->PSSetShaderResources(12, 1, &pSRV);
	DEVICECONTEXT->PSSetShaderResources(13, 1, &pSRV);
}


