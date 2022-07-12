#include "Framework.h"

DepthStencilState::DepthStencilState(): 
	mDesc{}, 
	mState(nullptr)
{
	mDesc.DepthEnable = true;
	mDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	mDesc.DepthFunc = D3D11_COMPARISON_LESS;

	mDesc.StencilEnable = true;
	mDesc.StencilReadMask = 0xff;
	mDesc.StencilWriteMask = 0xff;

	mDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	mDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	mDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	mDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	mDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	mDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	Changed();
}

DepthStencilState::~DepthStencilState()
{
	mState->Release();
}

void DepthStencilState::SetState(UINT stencilRef)
{
	DEVICECONTEXT->OMSetDepthStencilState(mState, stencilRef);
}

void DepthStencilState::DepthEnable(bool value)
{
	mDesc.DepthEnable = value;
	Changed();
}

void DepthStencilState::DepthWriteMask(D3D11_DEPTH_WRITE_MASK value)
{
	mDesc.DepthWriteMask = value;
	Changed();
}

void DepthStencilState::DepthFunc(D3D11_COMPARISON_FUNC value)
{
	mDesc.DepthFunc = value;
	Changed();
}

void DepthStencilState::Changed()
{
	if (mState != nullptr)
		mState->Release();

	V(DEVICE->CreateDepthStencilState(&mDesc, &mState));
}
