#include "Framework.h"

BlendState::BlendState() : 
	mState(nullptr), 
	mDesc{}
{
	mDesc.AlphaToCoverageEnable = false;
	mDesc.IndependentBlendEnable = false;

	mDesc.RenderTarget[0].BlendEnable = false; // �̰Ŵٸ���
	mDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	mDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	mDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	mDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	mDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA; // �̰Ŵٸ���
	mDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	mDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // 0x0f�� �Ǿ��ֱ��ѵ� �������� �ٸ����� �𸣰���.
																				// 2D �����̶� �����ϱ���.
	Change();
}

BlendState::~BlendState()
{
	mState->Release();
}

void BlendState::SetState()
{
	float blendFactor[] = { 0, 0, 0, 0 };
	DEVICECONTEXT->OMSetBlendState(mState, blendFactor, 0xffffffff);
}

void BlendState::SetAlpha(bool value)
{
	mDesc.RenderTarget[0].BlendEnable = value;
	mDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	mDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	mDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	Change();
}

void BlendState::SetAlphaToCoverage(bool value)
{
	mDesc.AlphaToCoverageEnable = value;

	Change();
}

void BlendState::SetAdditive()
{
	mDesc.RenderTarget[0].BlendEnable = true;
	mDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	mDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	mDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	Change();
}

void BlendState::Change()
{
	if (mState != nullptr)
		mState->Release();

	V(DEVICE->CreateBlendState(&mDesc, &mState));
}
